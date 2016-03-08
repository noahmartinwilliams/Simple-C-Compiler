#ifndef __PHASE_3_GENERATOR_STATEM_IF_STATEM_H__
#define __PHASE_3_GENERATOR_STATEM_IF_STATEM_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/generator-expr.h"
#include "generator/misc.h"
#include "globals.h"
#include "generator/types.h"
#include "parser/types.h"
#include "parser/funcs.h"
#include "parser/exprs.h"
#include "stack.h"
#include "optimizations.h"
#include "optimization-globals.h"
#include "generator/backend-exported.h"

static inline void generate_if_statement(FILE *fd, struct statem_t *s, struct reg_t *retu, struct expr_t *cond, struct statem_t *block) 
{
	unique_num++;
	cond=s->expr;
	block=s->left;
	struct statem_t *else_block=s->right;
	char *unique_name_else, *unique_name_true;
	asprintf(&unique_name_else, "if$not$true$%d", unique_num);
	asprintf(&unique_name_true, "if$true$%d", unique_num);

	place_comment(fd, "if (");
	#ifdef OPTIMIZE_IF_CONSTANT_CONDITION
	if (optimize_if_constant_condition && cond->kind==const_int && cond->attrs.cint_val!=0) {
		place_comment(fd, "1) {");
		generate_statement(fd, block);
		place_comment(fd, "}");
		return;
	} else if (optimize_if_constant_condition && cond->kind==const_int && cond->attrs.cint_val==0 && !block->has_gotos) {
		place_comment(fd, "0) {}");
		if (else_block!=NULL) {
			place_comment(fd, "else {");
			generate_statement(fd, else_block);
			place_comment(fd, "}");
		}
		return;
	}
	#endif

	#ifdef OPTIMIZE_CONDITION_TEST_DETECT
	char *op=cond->attrs.bin_op;
	if (optimize_condition_test_detect && cond->kind==bin_op && is_test_op(op)) {
		depth++;
		struct expr_t *left=cond->left, *right=cond->right;
		struct reg_t *left_reg=get_free_register(fd, get_type_size(left->type), depth, expr_is_float(left));
		retu=get_ret_register(get_type_size(left->type), expr_is_float(left));
		place_comment(fd, "(");
		place_comment(fd, "(");
		generate_expression(fd, left);
		assign_reg(fd, retu, left_reg);

		char *op_print=NULL;
		asprintf(&op_print, ") %s (", op);
		place_comment(fd, op_print);
		free(op_print);

		generate_expression(fd, right);
		place_comment(fd, ")");
		if (cond->right->type->body->core_type==_FLOAT) 
			compare_float_registers(fd, retu, left_reg);
		else
			compare_registers(fd, retu, left_reg);
		free_register(fd, left_reg);
		depth--;
		place_comment(fd, ") {");

		if (!strcmp("<", op))
			jmp_ge(fd, unique_name_else);
		else if (!strcmp(">", op))
			jmp_le(fd, unique_name_else);
		else if (!strcmp(">=", op))
			jmp_lt(fd, unique_name_else);
		else if (!strcmp("<=", op))
			jmp_gt(fd, unique_name_else);
		else if (!strcmp("==", op))
			jmp_neq(fd, unique_name_else);
		else if (!strcmp("!=", op))
			jmp_eq(fd, unique_name_else);

		generate_statement(fd, block);
		jmp(fd, unique_name_true);
		place_comment(fd, "} else {");
		place_label(fd, unique_name_else);
		if (else_block!=NULL)
			generate_statement(fd, else_block);

		place_comment(fd, "}");
		place_label(fd, unique_name_true);
		free(unique_name_else);
		free(unique_name_true);
		return;
	}
	#endif

	generate_expression(fd, cond);
	compare_register_to_int(fd, retu, 0);

	place_comment(fd, ")");

	jmp_eq(fd, unique_name_else);
	place_comment(fd, "{");
	generate_statement(fd, block);
	jmp(fd, unique_name_true);
	if (else_block==NULL) {
		place_comment(fd, "}");
		place_label(fd, unique_name_else);
	} else {
		place_label(fd, unique_name_else);
		place_comment(fd, "} else {");
		generate_statement(fd, else_block);
	}

	place_label(fd, unique_name_true);
	place_comment(fd, "}");
	free(unique_name_else);
	free(unique_name_true);
}

#endif
