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
		fprintf(fd, "\t#(\n\t#%s(\n", e->attrs.function->name);
		start_call(fd, e->attrs.function);
		if (e->attrs.function->num_arguments==0) {
			call(fd, e->attrs.function);
		} else {
			struct expr_t *current_e=e->right;
			struct reg_t *ret=get_ret_register(get_type_size(e->type));
			while (current_e!=NULL) {
				generate_expression(fd, current_e);
				add_argument(fd, ret, current_e->type);
				current_e=current_e->right;
			}
			call(fd, e->attrs.function);
		}
		fprintf(fd, "\t#)\n\t#)\n");
		/*TODO: Figure out how to handle struct return values */
	} else if (e->kind==const_str) {
		load_global_string(fd, e->attrs.cstr_val);
	}
}

void generate_pre_unary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret=get_ret_register(pointer_size);
	if (!strcmp(e->attrs.un_op, "&")) {
		get_address(fd, e->right);
	} else if (!strcmp(e->attrs.un_op, "*")) {
		fprintf(fd, "\t#(\n\t#*\n\t#(\n");
		generate_expression(fd, e->right);
		fprintf(fd, "\t#)\n");
		dereference(fd, ret, pointer_size);
		fprintf(fd, "\t#)\n");
	}
	depth--;
}


static void generate_comparison_expression(FILE *fd, struct expr_t *e, void (*comparitor)(FILE*, char*), char *true_string, char *false_string, struct reg_t *lhs)
{
	struct reg_t *ret=get_ret_register(e->type->body->size);

	fprintf(fd, "\t#(\n\t#(\n");
	generate_expression(fd, e->left);
	assign_reg(fd, ret, lhs);

	fprintf(fd, "\t#)\n\t#%s\n\t#(\n", e->attrs.bin_op);

	generate_expression(fd, e->right);
	fprintf(fd, "\t#)\n");
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

	fprintf(fd, "\t#)\n");

	free(is_true);
	free(is_false);

}

void generate_binary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret=get_ret_register(get_type_size(e->type));
	struct reg_t *lhs=get_free_register(fd, get_type_size(e->type));
	struct reg_t *rhs=get_free_register(fd, get_type_size(e->type));

	if (!strcmp(e->attrs.bin_op, "+")) {
		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);

		fprintf(fd, "\t#)\n\t#+\n\t#(\n");

		generate_expression(fd, e->right);

		assign_reg(fd, ret, rhs);
		fprintf(fd, "\t#)\n");

		int_add(fd, lhs, rhs);

		fprintf(fd, "\t#)\n");
	} else if (!strcmp(e->attrs.bin_op, "=")) {
		fprintf(fd, "\t#Note: lhs, and rhs of assignment is swapped\n");
		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->right);

		assign_reg(fd, ret, lhs);
		fprintf(fd, "\t#)\n\t#=\n\t#(\n");
		/* TODO: figure out a good way to abstract away the direct use
		 * of the mov command here. Printing opcodes is for handle-registers.c */
		 if (e->kind==pre_un_op && !strcmp(e->attrs.un_op, "*")) {
			generate_expression(fd, e->right);
			assign_dereference(fd, lhs, ret);
		 } else if (e->left->kind==var) {
			assign_var(fd, lhs, e->left->attrs.var);
		 }
		fprintf(fd, "\t#)\n\t#)\n");
	} else if (!strcmp(e->attrs.bin_op, "-")) {
		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);

		fprintf(fd, "\t#)\n\t#-\n\t#(\n");

		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);

		fprintf(fd, "\t#)\n");

		int_sub(fd, rhs, lhs);

		fprintf(fd, "\t#)\n");
	} else if (!strcmp(e->attrs.bin_op, "/")) {
		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);

		fprintf(fd, "\t#)\n\t#/\n\t#(\n");

		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);

		fprintf(fd, "\t#)\n");

		int_div(fd, lhs, rhs);

		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, "*")) {
		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);

		fprintf(fd, "\t#)\n\t#*\n\t#(\n");

		generate_expression(fd, e->right);

		fprintf(fd, "\t#)\n");

		int_mul(fd, ret, lhs);

		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, "==")) {
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

	} else if (!strcmp(e->attrs.bin_op, "<<")) {

		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);

		fprintf(fd, "\t#)\n\t#<<\n\t#(\n");

		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);

		fprintf(fd, "\t#)\n");
		shift_left(fd, lhs, rhs);

		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, ">>")) {

		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);

		fprintf(fd, "\t#)\n\t#>>\n\t#(\n");

		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);

		fprintf(fd, "\t#)\n");
		shift_right(fd, lhs, rhs);

		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, "+=")) {
		fprintf(fd, "\t#Note: lhs, and rhs of assignment is swapped\n");
		fprintf(fd, "\t#(\n\t#(\n");

		generate_expression(fd, e->right);
		assign_reg(fd, ret, lhs);

		fprintf(fd, "\t#)\n\t#+=\n\t#(\n");

		char *var=prepare_var_assignment(fd, e->left);

		fprintf(fd, "\t#)\n");

		int_inc_by(fd, lhs, var);

		fprintf(fd, "\t#)\n");
	} else if (!strcmp(e->attrs.bin_op, "|")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);
		fprintf(fd, "\t#)\n\t#|\n\t#(\n");

		generate_expression(fd, e->left);

		fprintf(fd, "\t#)\n");

		or(fd, ret, rhs);
		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, "&")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);

		fprintf(fd, "\t#)\n\t#&\n\t#(\n");

		generate_expression(fd, e->left);

		fprintf(fd, "\t#)\n");

		and(fd, ret, rhs);
		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, "^")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);

		fprintf(fd, "\t#)\n\t#^\n\t#(\n");

		generate_expression(fd, e->left);

		fprintf(fd, "\t#)\n");

		xor(fd, ret, rhs);
		fprintf(fd, "\t#)\n");
	}
	free_register(fd, rhs);
	free_register(fd, lhs);
	depth--;
}
