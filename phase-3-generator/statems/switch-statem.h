#ifndef __PHASE_3_GENERATOR_STATEMS_SWITCH_STATEM_H__
#define __PHASE_3_GENERATOR_STATEMS_SWITCH_STATEM_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/generator-expr.h"
#include "generator/misc.h"
#include "globals.h"
#include "generator/types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "handle-exprs.h"
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

	block=s->attrs._switch.cases;
	cond=s->attrs._switch.tester;
	struct reg_t *input=get_free_register(fd, get_type_size(cond->type), depth, false);

	place_comment(fd, "switch (");
	generate_expression(fd, cond);
	assign_reg(fd, retu, input);
	place_comment(fd, ") {");

	int x;
	int num=block->attrs.list.num;
	struct statem_t **statements=block->attrs.list.statements;
	bool found_default=false;
	struct statem_t *statement;
	struct statem_t *def=NULL;
	for (x=0; x<num; x++) {
		statement=statements[x];
		unique_num++;
		if (statements[x]->kind==_case) {
			place_comment(fd, "case: ");
			generate_expression(fd, statements[x]->attrs._case.condition);
			asprintf(&(statement->attrs._case.label), "switch$case$%d", unique_num);
			compare_registers(fd, retu, input);
			jmp_eq(fd, statement->attrs._case.label);
		} else  {
			found_default=true;
			place_comment(fd, "default: ");
			asprintf(&(statement->attrs._default.label), "switch$default$%d", unique_num);
			jmp(fd, statement->attrs._default.label);
			def=statement;
		}
	}

	for (x=0; x<num; x++) {
		statement=statements[x];
		if (statement->kind==_case) {
			place_label(fd, statement->attrs._case.label);
			generate_statement(fd, statement->attrs._case.block);
			free(statement->attrs._case.label);
		}
	}
	if (found_default) {
		place_label(fd, def->attrs._default.label);
		generate_statement(fd, statement->attrs._default.def);
		free(statement->attrs._default.label);
	}
	place_label(fd, loop_end);
	place_comment(fd, "}");
	free(loop_start);
	free(loop_end);
	free(pop(loop_stack));
}

#endif
