#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/generator-globals.h"
#include "generator/generator-misc.h"
#include "globals.h"
#include "generator/backend/backend.h"
#include "generator/generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"

void generate_binary_expression(FILE *fd, struct expr_t *e);
void generate_pre_unary_expression(FILE *fd, struct expr_t *e);
void generate_post_unary_expression(FILE *fd, struct expr_t *e);

void generate_expression(FILE *fd, struct expr_t *e)
{
	if (e->kind==bin_op)
		generate_binary_expression(fd, e);
	else if (e->kind==var)
		read_var(fd, e->attrs.var);
	else if (e->kind==const_int)
		assign_constant(fd, e);
	else if (e->kind==pre_un_op) {
		generate_pre_unary_expression(fd, e);
	} else if (e->kind==funccall) {
		struct func_t *f=e->attrs.function;
		place_comment(fd, "(");
		place_comment(fd, f->name);
		place_comment(fd, "(");
		start_call(fd, f);
		if (e->attrs.function->num_arguments==0) {
			call(fd, f);
		} else {
			struct expr_t *current_e=e->right;
			struct reg_t *ret=get_ret_register(get_type_size(current_e->type));
			while (current_e!=NULL) {
				generate_expression(fd, current_e->attrs.argument);
				add_argument(fd, ret, current_e->type);
				current_e=current_e->right;
			}
			call(fd, f);
		}
		place_comment(fd, ")");
		place_comment(fd, ")");
		/*TODO: Figure out how to handle struct return values */
	} else if (e->kind==const_str) {
		load_global_string(fd, e->attrs.cstr_val);
	} else if (e->kind==post_un_op) {
		generate_post_unary_expression(fd, e);
	}

	get_ret_register(get_type_size(e->type)); /* Sometimes a sub-expression will change the size of the return register. */
}

void generate_post_unary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	register struct reg_t *ret=get_ret_register(get_type_size(e->type));
	register struct reg_t *lhs=get_free_register(fd, get_type_size(e->type));
	place_comment(fd, "(");
	place_comment(fd, "(");
	generate_expression(fd, e->left);
	assign_reg(fd, ret, lhs);
	place_comment(fd, ")");

	register char *op=e->attrs.un_op;
	if (!strcmp(op, "++") ||  !strcmp(op, "--")) {
		if (!strcmp(op, "++"))
			int_inc(fd, ret);
		else if (!strcmp(op, "--"))
			int_dec(fd, ret);

		register struct expr_t *left=e->left;
		if (left->kind==pre_un_op && !strcmp(left->attrs.un_op, "*")) {
			struct reg_t *rhs=get_free_register(fd, get_type_size(e->type));
			struct reg_t *tmp=get_free_register(fd, get_type_size(e->type));
			assign_reg(fd, ret, tmp);
			generate_expression(fd, left->right);
			assign_reg(fd, ret, rhs);
			assign_reg(fd, tmp, ret);
			free_register(fd, tmp);

			assign_dereference(fd, ret, rhs);

			free_register(fd, rhs);
		} else if (left->kind==var) {
			assign_var(fd, ret, left->attrs.var);
		}
		assign_reg(fd, lhs, ret);
		place_comment(fd, e->attrs.un_op);
	}
	place_comment(fd, ")");
	free_register(fd, lhs);
	depth--;
}
void generate_pre_unary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	size_t s=get_type_size(e->right->type);

	register struct reg_t *ret=get_ret_register(s);
	register char *op=e->attrs.un_op;
	place_comment(fd, "(");
	place_comment(fd, op);
	place_comment(fd, "(");

	if (!strcmp(op, "&")) {
		get_address(fd, e->right);
		place_comment(fd, ")");
	} else {
		generate_expression(fd, e->right);
		place_comment(fd, ")");
		if (!strcmp(op, "*"))
			dereference(fd, ret, pointer_size);

		else if (!strcmp(op, "!"))
			test_invert(fd, ret);

		else if (!strcmp(op, "++")) {
			
			int_inc(fd, ret);
			if (e->right->kind==pre_un_op && !strcmp(e->right->attrs.un_op, "*")) {
				register struct reg_t *rhs=get_free_register(fd, pointer_size);
				register struct reg_t *tmp=get_free_register(fd, get_type_size(e->type));
				assign_reg(fd, ret, tmp);

				ret=get_ret_register(pointer_size);
				generate_expression(fd, e->right->right);

				assign_reg(fd, ret, rhs);
				ret=get_ret_register(get_type_size(e->type));
				assign_reg(fd, tmp, ret);

				free_register(fd, tmp);

				assign_dereference(fd, ret, rhs);

				free_register(fd, rhs);
			} else if (e->right->kind==var)
				assign_var(fd, ret, e->right->attrs.var);

		} else if (!strcmp(op, "~")) 
			invert(fd, ret);
	}
	place_comment(fd, ")");
	depth--;
}


static void generate_comparison_expression(FILE *fd, struct expr_t *e, void (*comparitor)(FILE*, char*), char *true_string, char *false_string, struct reg_t *lhs)
{
	depth++;
	struct reg_t *ret=get_ret_register(get_type_size(e->type));

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
	depth--;

}

void generate_binary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret=get_ret_register(get_type_size(e->type));
	struct reg_t *lhs, *rhs;
	char *op=e->attrs.bin_op;
	if (!strcmp(op, "=")) {
		/* The reason for redoing rhs, and lhs in the else statement is that 
		this way I can declare lhs AFTER I've generated the 
		right hand side of the expression which leaves whatever
		register I choose available for evaulation of the rhs */
		place_comment(fd, "Note: lhs, and rhs of assignment is swapped");
		place_comment(fd, "(");
		place_comment(fd, "(");

		generate_expression(fd, e->right);
		lhs=get_free_register(fd, get_type_size(e->type));

		assign_reg(fd, ret, lhs);
		place_comment(fd, ")");
		place_comment(fd, "=");
		place_comment(fd, "(");
		/* TODO: figure out a good way to abstract away the direct use
		 * of the mov command here. Printing opcodes is for handle-registers.c */
		struct expr_t *left=e->left;
		if (left->kind==pre_un_op && !strcmp(left->attrs.un_op, "*")) {
			generate_expression(fd, left->right);
			assign_dereference(fd, lhs, ret);
		} else if (left->kind==var) {
			assign_var(fd, lhs, left->attrs.var);
		}
		place_comment(fd, ")");
		place_comment(fd, ")");
		free_register(fd, lhs);
	} else {
		size_t size=get_type_size(e->type);
		lhs=get_free_register(fd, size);
		rhs=get_free_register(fd, size);
		if (!strcmp(op, "==")) {
			generate_comparison_expression(fd, e, jmp_eq, "is$eq$%d", "is$not$eq$%d", lhs);

		} else if (!strcmp(op, "<")) {
			generate_comparison_expression(fd, e, jmp_lt, "is$lt$%d", "is$not$lt$%d", lhs);

		} else if (!strcmp(op, ">")) {
			generate_comparison_expression(fd, e, jmp_gt, "is$gt$%d", "is$not$gt$%d", lhs);

		} else if (!strcmp(op, "!=")) {
			generate_comparison_expression(fd, e, jmp_neq, "is$ne$%d", "is$eq$%d", lhs);

		} else if (!strcmp(op, ">=")) {
			generate_comparison_expression(fd, e, jmp_ge, "is$ge$%d", "is$lt$%d", lhs);

		} else if (!strcmp(op, "<=")) {
			generate_comparison_expression(fd, e, jmp_le, "is$le$%d", "is$gt$%d", lhs);

		} else if (!strcmp(op, "+=")) {
			place_comment(fd, "Note: lhs, and rhs of assignment is swapped");
			place_comment(fd, "(");
			place_comment(fd, "(");

			generate_expression(fd, e->right);
			assign_reg(fd, ret, lhs);

			place_comment(fd, ")");
			place_comment(fd, "+=");
			place_comment(fd, "(");

			generate_expression(fd, e->left);
			int_add(fd, ret, lhs);

			if (e->left->kind==pre_un_op && !strcmp(e->left->attrs.un_op, "*")) {
				assign_reg(fd, ret, lhs);
				generate_expression(fd, e->left);
				assign_dereference(fd, lhs, ret);
			} else if (e->left->kind==var)
				assign_var(fd, ret, e->left->attrs.var);

			place_comment(fd, ")");
			place_comment(fd, ")");
		} else { 
			place_comment(fd, "(");
			place_comment(fd, "(");

			generate_expression(fd, e->left);
			assign_reg(fd, ret, lhs);

			place_comment(fd, ")");
			place_comment(fd, op);
			place_comment(fd, "(");

			generate_expression(fd, e->right);

			place_comment(fd, ")");
			if (!strcmp(op, "+"))
				int_add(fd, lhs, ret);
			else if (!strcmp(op, "-")) 
				int_sub(fd, lhs, ret);
			else if (!strcmp(op, "/"))
				int_div(fd, lhs, ret);
			else if (!strcmp(op, "*"))
				int_mul(fd, lhs, ret);
			else if (!strcmp(op, "<<"))
				shift_left(fd, lhs, ret);
			else if (!strcmp(op, ">>"))
				shift_right(fd, lhs, ret);
			else if (!strcmp(op, "|"))
				or(fd, lhs, ret);
			else if (!strcmp(op, "&"))
				and(fd, lhs, ret);
			else if (!strcmp(op, "^"))
				xor(fd, lhs, ret);
			else if (!strcmp(op, "||"))
				test_or(fd, lhs, ret);
			else if (!strcmp(op, "&&"))
				test_and(fd, lhs, ret);
			place_comment(fd, ")");
		}
		free_register(fd, rhs);
		free_register(fd, lhs);
	}
	depth--;
}
