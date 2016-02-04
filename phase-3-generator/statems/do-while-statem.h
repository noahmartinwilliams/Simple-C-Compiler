#ifndef __PHASE_3_GENERATOR_STATEMS_DO_WHILE_STATEM_H__
#define __PHASE_3_GENERATOR_STATEMS_DO_WHILE_STATEM_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/generator-globals.h"
#include "generator/generator-expr.h"
#include "generator/generator-misc.h"
#include "globals.h"
#include "generator/generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "handle-exprs.h"
#include "stack.h"
#include "optimizations.h"
#include "optimization-globals.h"
#include "generator/backend-exported.h"

static inline void generate_do_while_loop(FILE *fd, struct statem_t *s, struct reg_t *retu, struct expr_t *cond, struct statem_t *block) 
{
	int *l=malloc(sizeof(int));
	unique_num++;
	*l=unique_num;
	push(loop_stack, l);

	char *loop_start, *loop_end;
	cond=s->attrs.do_while.condition;
	block=s->attrs.do_while.block;
	asprintf(&loop_start, "loop$start$%d", unique_num);
	asprintf(&loop_end, "loop$end$%d", unique_num);

	place_comment(fd, "do {");

	place_label(fd, loop_start);
	generate_statement(fd, block);

	place_comment(fd, "} while (");
	#ifdef OPTIMIZE_DO_WHILE_CONSTANT_CONDITION
	if (optimize_do_while_constant_condition && cond->kind==const_int && cond->attrs.cint_val!=0) {
		place_comment(fd, "1) ;");
		jmp(fd, loop_start);
		place_label(fd, loop_end);
		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));
		return;
	} else if (optimize_do_while_constant_condition && cond->kind==const_int && cond->attrs.cint_val==0) {
		place_comment(fd, "0) ;");
		place_label(fd, loop_end);
		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));
		return;
	}
	#endif
	generate_expression(fd, cond);
	compare_register_to_int(fd, retu, 0);
	jmp_neq(fd, loop_start);

	place_comment(fd, ") ;");
	place_label(fd, loop_end);
	free(loop_start);
	free(loop_end);
	free(pop(loop_stack));
}

#endif
