#ifndef __PHASE_3_GENERATOR_STATEMS_FOR_STATEM_H__
#define __PHASE_3_GENERATOR_STATEMS_FOR_STATEM_H__
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

static inline void generate_for_loop(FILE *fd, struct statem_t *s, struct reg_t *retu, struct expr_t *cond, struct statem_t *block) 
{
	cond=s->expr;
	block=s->right;
	struct expr_t *update=s->attrs._for.update;
	struct expr_t *initial=s->attrs._for.initial;

	place_comment(fd, "for (");
	if (initial!=NULL)
		generate_expression(fd, initial);

	place_comment(fd, "; ");

	int *l=malloc(sizeof(int));
	unique_num++;
	*l=unique_num;
	push(loop_stack, l);

	char *loop_start, *loop_end;
	asprintf(&loop_start, "loop$start$%d", unique_num);
	asprintf(&loop_end, "loop$end$%d", unique_num);

	place_label(fd, loop_start);
	#ifdef OPTIMIZE_FOR_CONSTANT_CONDITION
	if(optimize_for_constant_condition && cond->kind==const_int && cond->attrs.cint_val==0) {
		
		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));
		place_comment(fd, "0) {}");
		return;
	} else if (optimize_for_constant_condition && cond->kind==const_int && cond->attrs.cint_val!=0) {
		place_comment(fd, "1) {");
		generate_statement(fd, block);

		generate_expression(fd, update);
		jmp(fd, loop_start);
		place_comment(fd, "}");
		place_label(fd, loop_end);

		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));
		return;
	}
	#endif
	generate_expression(fd, cond);

	compare_register_to_int(fd, retu, 0);
	jmp_eq(fd, loop_end);
	place_comment(fd, ") {");

	generate_statement(fd, block);

	if (update!=NULL)
		generate_expression(fd, update);

	jmp(fd, loop_start);
	place_comment(fd, "}");
	place_label(fd, loop_end);

	free(loop_start);
	free(loop_end);
	free(pop(loop_stack));
}

#endif
