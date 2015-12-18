#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator-globals.h"
#include "generator-misc.h"
#include "globals.h"
#include "backend.h"
#include "generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"

void generate_binary_expression(FILE *fd, struct expr_t *e);
void generate_pre_unary_expression(FILE *fd, struct expr_t *e);

void generate_expression(FILE *fd, struct expr_t *e)
{
	if (e->kind==bin_op) {
		generate_binary_expression(fd, e);
	} else if (e->kind==var) {
		read_var(fd, e->attrs.var);
	} else if (e->kind==const_int) {
		assign_constant(fd, e);
	} else if (e->kind==pre_un_op) {
		generate_pre_unary_expression(fd, e);
	} else if (e->kind==funccall) {
		place_comment(fd, "(");
		place_comment(fd, e->attrs.function->name);
		place_comment(fd, "(");
		start_call(fd, e->attrs.function);
		if (e->attrs.function->num_arguments==0) {
			call(fd, e->attrs.function);
		} else {
			struct expr_t *current_e=e->right;
			struct reg_t *ret=get_ret_register(get_type_size(current_e->type));
			while (current_e!=NULL) {
				generate_expression(fd, current_e);
				add_argument(fd, ret, current_e->type);
				current_e=current_e->right;
			}
			call(fd, e->attrs.function);
		}
		place_comment(fd, ")");
		place_comment(fd, ")");
		/*TODO: Figure out how to handle struct return values */
	} else if (e->kind==const_str) {
		load_global_string(fd, e->attrs.cstr_val);
	}
}

void generate_pre_unary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	size_t s=0;
	if (e->type==NULL)
		s=word_size;
	else
		s=get_type_size(e->right->type);

	struct reg_t *ret=get_ret_register(s);
	if (!strcmp(e->attrs.un_op, "&")) {
		place_comment(fd, "(");
		place_comment(fd, "&");
		place_comment(fd, "(");
		get_address(fd, e->right);
		place_comment(fd, ")");
		place_comment(fd, ")");
	} else if (!strcmp(e->attrs.un_op, "*")) {
		place_comment(fd, "(");
		place_comment(fd, "*");
		place_comment(fd, "(");
		generate_expression(fd, e->right);
		place_comment(fd, ")");
		dereference(fd, ret, pointer_size);
		place_comment(fd, ")");
	} else if (!strcmp(e->attrs.un_op, "!")) {
		place_comment(fd, "(");
		place_comment(fd, "!");
		place_comment(fd, "(");

		generate_expression(fd, e->right);
		
		place_comment(fd, ")");

		test_invert(fd, ret);

		place_comment(fd, ")");
	}
	depth--;
}


static void generate_comparison_expression(FILE *fd, struct expr_t *e, void (*comparitor)(FILE*, char*), char *true_string, char *false_string, struct reg_t *lhs)
{
	struct reg_t *ret=get_ret_register(e->type->body->size);

	place_comment(fd, "(");
	place_comment(fd, "(");
	generate_expression(fd, e->left);
	assign_reg(fd, ret, lhs);

	place_comment(fd, ")");
	place_comment(fd, e->attrs.bin_op);
	place_comment(fd, "(");

	generate_expression(fd, e->right);
	place_comment(fd, ")");
	compare_registers(fd, ret, lhs);

	unique_num++;
	char *is_true, *is_false;
	asprintf(&is_true, true_string, unique_num);
	asprintf(&is_false, false_string, unique_num);

	comparitor(fd, is_true);
	assign_constant_int(fd, 0);
	jmp(fd, is_false);

	place_label(fd, is_true);
	assign_constant_int(fd, 1);
	place_label(fd, is_false);

	place_comment(fd, ")");

	free(is_true);
	free(is_false);

}

void generate_binary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret=get_ret_register(get_type_size(e->type));
	struct reg_t *lhs=get_free_register(fd, get_type_size(e->type));
	struct reg_t *rhs=get_free_register(fd, get_type_size(e->type));

	if (!strcmp(e->attrs.bin_op, "==")) {
		generate_comparison_expression(fd, e, jmp_eq, "is$eq$%d", "is$not$eq$%d", lhs);

	} else if (!strcmp(e->attrs.bin_op, "<")) {
		generate_comparison_expression(fd, e, jmp_lt, "is$lt$%d", "is$not$lt$%d", lhs);

	} else if (!strcmp(e->attrs.bin_op, ">")) {
		generate_comparison_expression(fd, e, jmp_gt, "is$gt$%d", "is$not$gt$%d", lhs);

	} else if (!strcmp(e->attrs.bin_op, "!=")) {
		generate_comparison_expression(fd, e, jmp_neq, "is$ne$%d", "is$eq$%d", lhs);

	} else if (!strcmp(e->attrs.bin_op, ">=")) {
		generate_comparison_expression(fd, e, jmp_ge, "is$ge$%d", "is$lt$%d", lhs);

	} else if (!strcmp(e->attrs.bin_op, "<=")) {
		generate_comparison_expression(fd, e, jmp_le, "is$le$%d", "is$gt$%d", lhs);

	} else if (!strcmp(e->attrs.bin_op, "=")) {
		place_comment(fd, "Note: lhs, and rhs of assignment is swapped");
		place_comment(fd, "(");
		place_comment(fd, "(");

		generate_expression(fd, e->right);

		assign_reg(fd, ret, lhs);
		place_comment(fd, ")");
		place_comment(fd, "=");
		place_comment(fd, ")");
		/* TODO: figure out a good way to abstract away the direct use
		 * of the mov command here. Printing opcodes is for handle-registers.c */
		 if (e->kind==pre_un_op && !strcmp(e->attrs.un_op, "*")) {
			generate_expression(fd, e->right);
			assign_dereference(fd, lhs, ret);
		 } else if (e->left->kind==var) {
			assign_var(fd, lhs, e->left->attrs.var);
		 }
		place_comment(fd, ")");
		place_comment(fd, ")");
	} else if (!strcmp(e->attrs.bin_op, "+=")) {
		place_comment(fd, "Note: lhs, and rhs of assignment is swapped");
		place_comment(fd, "(");
		place_comment(fd, "(");

		generate_expression(fd, e->right);
		assign_reg(fd, ret, lhs);

		place_comment(fd, ")");
		place_comment(fd, "+=");
		place_comment(fd, "(");

		char *var=prepare_var_assignment(fd, e->left);

		place_comment(fd, ")");

		int_inc_by(fd, lhs, var);

		place_comment(fd, ")");
	} else { 
		place_comment(fd, "(");
		place_comment(fd, "(");

		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);

		place_comment(fd, ")");
		place_comment(fd, e->attrs.bin_op);
		place_comment(fd, "(");

		generate_expression(fd, e->right);

		place_comment(fd, ")");
		char *c=e->attrs.bin_op;
		if (!strcmp(c, "+"))
			int_add(fd, lhs, ret);
		else if (!strcmp(c, "-")) 
			int_sub(fd, lhs, ret);
		else if (!strcmp(c, "/"))
			int_div(fd, lhs, ret);
		else if (!strcmp(c, "*"))
			int_mul(fd, lhs, ret);
		else if (!strcmp(c, "<<"))
			shift_left(fd, lhs, ret);
		else if (!strcmp(c, ">>"))
			shift_right(fd, lhs, ret);
		else if (!strcmp(c, "|"))
			or(fd, lhs, ret);
		else if (!strcmp(c, "&"))
			and(fd, lhs, ret);
		else if (!strcmp(c, "^"))
			xor(fd, lhs, ret);
		else if (!strcmp(c, "||"))
			test_or(fd, lhs, ret);
		else if (!strcmp(c, "&&"))
			test_and(fd, lhs, ret);
		place_comment(fd, ")");
	}
	free_register(fd, rhs);
	free_register(fd, lhs);
	depth--;
}
