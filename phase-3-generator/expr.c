#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/misc.h"
#include "globals.h"
#include "generator/types.h"
#include "generator/backend-exported.h"
#include "utilities/types.h"
#include "utilities/funcs.h"

static inline void generate_binary_expression(FILE *fd, struct expr_t *e);
static inline void generate_pre_unary_expression(FILE *fd, struct expr_t *e);
static inline void generate_post_unary_expression(FILE *fd, struct expr_t *e);

void generate_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret;
	if (e==NULL)
		return;

	if (e->kind==bin_op)
		generate_binary_expression(fd, e);
	else if (e->kind==var) {
		if (e->attrs.var->type->body->core_type==_FLOAT)
			read_float_var(fd, e->attrs.var);
		else
			read_var(fd, e->attrs.var);
	} else if (e->kind==const_int)
		assign_constant(fd, e);
	else if (e->kind==const_float)
		assign_constant_float(fd, e);
	else if (e->kind==const_size_t)
		assign_constant(fd, e);
	else if (e->kind==pre_un_op)
		generate_pre_unary_expression(fd, e);
	else if (e->kind==funccall) {
		struct func_t *f=e->attrs.function;
		place_comment(fd, "(");
		place_comment(fd, f->name);
		place_comment(fd, "(");
		start_call(fd, f);
		if (e->attrs.function->num_arguments==0) {
			if (f->do_inline) {
				generate_statement(fd, f->statement_list);
			} else {
				call(fd, f);
			}
		} else {
			struct expr_t *current_e=e->right;
			while (current_e!=NULL) {
				place_comment(fd, "(");
				generate_expression(fd, current_e->attrs.argument);
				ret=get_ret_register(get_type_size(current_e->type), expr_is_float(current_e));
				add_argument(fd, ret, current_e->type);
				current_e=current_e->right;
				place_comment(fd, ")");
				place_comment(fd, ", ");
			}
			call(fd, f);
		}
		place_comment(fd, ")");
		place_comment(fd, ")");
		/*TODO: Figure out how to handle struct return values */
	} else if (e->kind==const_str)
		load_global_string(fd, e->attrs.cstr_val);
	else if (e->kind==post_un_op)
		generate_post_unary_expression(fd, e);
	else if (e->kind==func_ptr_call) {
		ret=get_ret_register(pointer_size, false);
		read_var(fd, e->attrs.var);
		struct reg_t *holder=get_free_register(fd, pointer_size, depth, false);
		place_comment(fd, "(");
		place_comment(fd, e->attrs.var->name);
		assign_reg(fd, ret, holder);
		place_comment(fd, "(");
		start_func_ptr_call(fd, ret);
		if (e->attrs.function->num_arguments==0) {
			call_function_pointer(fd, holder);
		} else {
			struct expr_t *current_e=e->right;
			while (current_e!=NULL) {
				generate_expression(fd, current_e->attrs.argument);
				add_argument(fd, ret, current_e->type);
				current_e=current_e->right;
			}
			call_function_pointer(fd, holder);
		}
		place_comment(fd, ")");
		place_comment(fd, ")");
		/*TODO: Figure out how to handle struct return values */
	} else if (e->kind==func_val) {
		ret=get_ret_register(pointer_size, expr_is_float(e));
		place_comment(fd, "(");
		load_function_ptr(fd, e->attrs.function, ret);
		place_comment(fd, ")");
	} else if (e->kind==convert && e->type->num_arrays > 0 && e->right->type->num_arrays> 0 && get_type_size(e->type) == get_type_size(e->right->type)) {
		
		generate_expression(fd, e->right);
	} else if (e->kind==convert && e->type->body->core_type==_INT && e->right->type->body->core_type==_FLOAT) {
		generate_expression(fd, e->right);
		convert_float_to_int(fd, get_ret_register(get_type_size(e->type), true), get_ret_register(get_type_size(e->type), false));
	} else if (e->kind==convert && e->type->body->core_type==e->right->type->body->core_type && e->type->body->core_type==_INT) {
		char *comment;
		asprintf(&comment, "((%s)", e->type->name);
		place_comment(fd, comment);
		free(comment);

		ret=get_ret_register(get_type_size(e->type), false);
		struct reg_t *tmp=get_free_register(fd, get_type_size(e->right->type), depth, false);
		generate_expression(fd, e->right);
		assign_reg(fd, ret, tmp);
		convert_int_size(fd, tmp, get_type_size(e->type), e->right->type->is_signed);
		free_register(fd, tmp);
		
		place_comment(fd, ")");
	}

	get_ret_register(get_type_size(e->type), expr_is_float(e)); /* Sometimes a sub-expression will change the size of the return register. */
	depth--;
}

static inline void generate_post_unary_expression(FILE *fd, struct expr_t *e)
{
	struct reg_t *ret=get_ret_register(get_type_size(e->type), expr_is_float(e));
	struct reg_t *lhs=get_free_register(fd, get_type_size(e->type), depth, expr_is_float(e));
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
			struct reg_t *rhs=get_free_register(fd, get_type_size(e->type), depth, expr_is_float(e));
			struct reg_t *tmp=get_free_register(fd, get_type_size(e->type), depth, expr_is_float(e));
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
}

static inline void generate_pre_unary_expression(FILE *fd, struct expr_t *e)
{
	size_t s=get_type_size(e->right->type);

	register struct reg_t *ret=get_ret_register(s, expr_is_float(e));
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
				register struct reg_t *rhs=get_free_register(fd, pointer_size, depth, false);
				register struct reg_t *tmp=get_free_register(fd, get_type_size(e->type), depth, false);
				assign_reg(fd, ret, tmp);

				ret=get_ret_register(pointer_size, false);
				generate_expression(fd, e->right->right);

				assign_reg(fd, ret, rhs);
				ret=get_ret_register(get_type_size(e->type), false);
				assign_reg(fd, tmp, ret);

				free_register(fd, tmp);

				assign_dereference(fd, ret, rhs);

				free_register(fd, rhs);
			} else if (e->right->kind==var)
				assign_var(fd, ret, e->right->attrs.var);

		} else if (!strcmp(op, "~")) 
			invert(fd, ret);
		else if (!strcmp(op, "-"))
			int_neg(fd, ret);
	}
	place_comment(fd, ")");
}


static inline void generate_comparison_expression(FILE *fd, struct expr_t *e, void (*comparitor)(FILE*, char*), char *true_string, char *false_string, struct reg_t *lhs)
{
	struct reg_t *ret=get_ret_register(get_type_size(e->left->type), expr_is_float(e->left));

	place_comment(fd, "(");
	place_comment(fd, "(");
	generate_expression(fd, e->left);
	assign_reg(fd, ret, lhs);

	place_comment(fd, ")");
	place_comment(fd, e->attrs.bin_op);
	place_comment(fd, "(");

	generate_expression(fd, e->right);
	place_comment(fd, ")");
	bool is_float=e->right->type->body->core_type==_FLOAT;
	if (!is_float)
		compare_registers(fd, ret, lhs);
	else
		compare_float_registers(fd, ret, lhs);

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


static inline void generate_binary_expression(FILE *fd, struct expr_t *e)
{
	struct reg_t *ret=get_ret_register(get_type_size(e->type), expr_is_float(e));
	struct reg_t *lhs, *rhs;
	char *op=e->attrs.bin_op;

	if (!strcmp(op, "=")) {
		struct expr_t *left=e->left;
		place_comment(fd, "Note: lhs, and rhs of assignment is swapped");
		place_comment(fd, "(");
		place_comment(fd, "(");

		generate_expression(fd, e->right);
		lhs=get_free_register(fd, get_type_size(e->type), depth, expr_is_float(e));

		assign_reg(fd, ret, lhs);
		place_comment(fd, ") = (");

		if (left->kind==pre_un_op && !strcmp(left->attrs.un_op, "*")) {

			place_comment(fd, "* (");
			generate_expression(fd, left->right);
			assign_dereference(fd, lhs, ret);
			place_comment(fd, ")");

		} else if (left->kind==var)
			assign_var(fd, lhs, left->attrs.var);
		place_comment(fd, ")");
		place_comment(fd, ")");
		free_register(fd, lhs);
	} else if (!strcmp(op, ",")) {
		place_comment(fd, "(");
		place_comment(fd, "(");
		generate_expression(fd, e->left);
		place_comment(fd, ") , (");
		generate_expression(fd, e->right);
		place_comment(fd, ")");
		place_comment(fd, ")");
	} else {
		size_t size=get_type_size(e->type);
		lhs=get_free_register(fd, get_type_size(e->left->type), depth, expr_is_float(e->left));
		rhs=get_free_register(fd, get_type_size(e->right->type), depth, expr_is_float(e->left));
		if (!strcmp(op, "==")) 
			generate_comparison_expression(fd, e, jmp_eq, "is$eq$%d", "is$not$eq$%d", lhs);
		else if (!strcmp(op, "<"))
			generate_comparison_expression(fd, e, jmp_lt, "is$lt$%d", "is$not$lt$%d", lhs);
		else if (!strcmp(op, ">"))
			generate_comparison_expression(fd, e, jmp_gt, "is$gt$%d", "is$not$gt$%d", lhs);
		else if (!strcmp(op, "!="))
			generate_comparison_expression(fd, e, jmp_neq, "is$ne$%d", "is$eq$%d", lhs);
		else if (!strcmp(op, ">="))
			generate_comparison_expression(fd, e, jmp_ge, "is$ge$%d", "is$lt$%d", lhs);
		else if (!strcmp(op, "<="))
			generate_comparison_expression(fd, e, jmp_le, "is$le$%d", "is$gt$%d", lhs);
		else if (!strcmp(op, "?")) {
			unique_num++;
			place_comment(fd, "(");
			place_comment(fd, "(");
			generate_expression(fd, e->left);
			assign_reg(fd, ret, lhs);
			place_comment(fd, ") ?");

			char *then, *_else;
			asprintf(&then, "expression$then$%d", unique_num);
			asprintf(&_else, "expression$else$%d", unique_num);

			compare_register_to_int(fd, ret, 0);
			jmp_eq(fd, _else);
			place_comment(fd, "(");
			place_comment(fd, "(");
			generate_expression(fd, e->right->left);
			jmp(fd, then);
			place_comment(fd, ")");

			place_comment(fd, ": (");
			place_label(fd, _else);
			generate_expression(fd, e->right->right);
			place_comment(fd, ")");
			place_label(fd, then);
			place_comment(fd, ")");
			place_comment(fd, ")");

			free(then);
			free(_else);
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
			if (!strcmp(op, "+")) {
				add(fd, lhs, ret);
			} else if (!strcmp(op, "-")) {
				sub(fd, lhs, ret);
			} else if (!strcmp(op, "*")) {
				mul(fd, lhs, ret);
			} else if (!strcmp(op, "/")) {
				_div(fd, lhs, ret);
			} else if (e->type->body->core_type==_INT) {
				if (!strcmp(op, "<<"))
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
				else if (!strcmp(op, "%"))
					int_num(fd, lhs, ret);
			} 
			place_comment(fd, ")");
		}
		free_register(fd, rhs);
		free_register(fd, lhs);
	}
}
