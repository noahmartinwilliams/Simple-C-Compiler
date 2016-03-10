#ifndef __PHASE_3_GENERATOR_STATEMS_SWITCH_STATEM_H__
#define __PHASE_3_GENERATOR_STATEMS_SWITCH_STATEM_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/expr.h"
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

static inline void generate_switch_statement(FILE *fd, struct statem_t *s, struct reg_t *retu, struct expr_t *cond, struct statem_t *block) 
{
	int *l=malloc(sizeof(int));
	unique_num++;
	*l=unique_num;
	push(loop_stack, l);
	char *loop_start, *loop_end;
	asprintf(&loop_start, "loop$start$%d", unique_num);
	asprintf(&loop_end, "loop$end$%d", unique_num);
	place_label(fd, loop_start);

	block=s->right;
	cond=s->expr;
	struct reg_t *input=get_free_register(fd, get_type_size(cond->type), depth, false);

	place_comment(fd, "switch (");
	generate_expression(fd, cond);
	assign_reg(fd, retu, input);
	place_comment(fd, ") {");

	int x;
	struct statem_t *statement=block;
	bool found_default=false;
	struct statem_t *def=NULL;
	for (; statement!=NULL; statement=statement->right) {
		unique_num++;
		if (statement->left->kind==_case) {
			place_comment(fd, "case: ");
			generate_expression(fd, statement->left->expr);
			asprintf(&(statement->left->attrs.label_name), "switch$case$%d", unique_num);
			compare_registers(fd, retu, input);
			jmp_eq(fd, statement->left->attrs.label_name);
		} else  {
			found_default=true;
			place_comment(fd, "default: ");
			asprintf(&(statement->left->attrs.label_name), "switch$default$%d", unique_num);
			jmp(fd, statement->left->attrs.label_name);
			def=statement->left;
		}
	}

	for (statement=block; statement!=NULL; statement=statement->right) {
		if (statement->left->kind==_case) {
			place_label(fd, statement->left->attrs.label_name);
			generate_statement(fd, statement->left->right);
			free(statement->left->attrs.label_name);
		}
	}
	if (found_default) {
		place_label(fd, def->attrs.label_name);
		generate_statement(fd, def->right);
		free(def->attrs.label_name);
	}
	place_label(fd, loop_end);
	place_comment(fd, "}");
	free(loop_start);
	free(loop_end);
	free(pop(loop_stack));
}

#endif
