#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/generator-globals.h"
#include "generator/generator-expr.h"
#include "generator/generator-misc.h"
#include "globals.h"
#include "generator/backend/backend.h"
#include "generator/generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"
#include "optimizations.h"
#include "optimization-globals.h"

void generate_statement(FILE *fd, struct statem_t *s);

static inline void generate_while_loop(FILE *fd, struct statem_t *s, struct reg_t *retu, struct expr_t *cond, struct statem_t *block)
{
	cond=s->attrs._while.condition;
	block=s->attrs._while.block;
	int *l=malloc(sizeof(int));
	unique_num++;
	*l=unique_num;
	push(loop_stack, l);

	char *loop_start, *loop_end;
	asprintf(&loop_start, "loop$start$%d", unique_num);
	asprintf(&loop_end, "loop$end$%d", unique_num);

	place_comment(fd, "while (");
	place_label(fd, loop_start);
	#ifdef OPTIMIZE_WHILE_CONSTANT_CONDITION
	if (optimize_while_constant_condition && cond->kind==const_int && cond->attrs.cint_val!=0) {
		
		place_comment(fd, "1) {");
		generate_statement(fd, block);
		jmp(fd, loop_start);
		place_comment(fd, "}");
		place_label(fd, loop_end);

		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));
		return;
	} else if ( optimize_while_constant_condition && cond->kind==const_int && cond->attrs.cint_val==0 ) {
		place_comment(fd, "0) {} Nothing done! :D");
		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));
		return;
	}
	generate_expression(fd, cond);
	#endif

	compare_register_to_int(fd, retu, 0);
	jmp_eq(fd, loop_end);
	place_comment(fd, ") {");

	generate_statement(fd, block);

	jmp(fd, loop_start);
	place_comment(fd, "}");
	place_label(fd, loop_end);

	free(loop_start);
	free(loop_end);
	free(pop(loop_stack));

}

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

static inline void generate_if_statement(FILE *fd, struct statem_t *s, struct reg_t *retu, struct expr_t *cond, struct statem_t *block) 
{
	cond=s->attrs._if.condition;
	block=s->attrs._if.block;
	struct statem_t *else_block=s->attrs._if.else_block;

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

	generate_expression(fd, cond);
	compare_register_to_int(fd, retu, 0);

	unique_num++;
	char *unique_name_else, *unique_name_true;
	asprintf(&unique_name_else, "if$not$true$%d", unique_num);
	asprintf(&unique_name_true, "if$true$%d", unique_num);
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

static inline void generate_for_loop(FILE *fd, struct statem_t *s, struct reg_t *retu, struct expr_t *cond, struct statem_t *block) 
{
	cond=s->attrs._for.cond;
	block=s->attrs._for.block;
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
	struct reg_t *input=get_free_register(fd, get_type_size(cond->type));

	place_comment(fd, "switch (");
	generate_expression(fd, cond);
	assign_reg(fd, retu, input);
	place_comment(fd, ") {");

	int x;
	int num=block->attrs.list.num;
	struct statem_t **statements=block->attrs.list.statements;
	for (x=0; x<num; x++) {
		unique_num++;
		place_comment(fd, "case ");
		generate_expression(fd, statements[x]->attrs._case.condition);
		asprintf(&(statements[x]->attrs._case.label), "switch$case$%d", unique_num);
		compare_registers(fd, retu, input);
		jmp_eq(fd, statements[x]->attrs._case.label);
	}

	for (x=0; x<num; x++) {
		place_label(fd, statements[x]->attrs._case.label);
		generate_statement(fd, statements[x]->attrs._case.block);
		free(statements[x]->attrs._case.label);
	}
	place_comment(fd, "}");
	free(loop_start);
	free(loop_end);
	free(pop(loop_stack));
}
void generate_statement(FILE *fd, struct statem_t *s)
{
	depth++;
	struct reg_t *retu=get_ret_register(word_size);
	struct expr_t *cond;
	struct statem_t *block;
	if (s->kind==expr) {
		generate_expression(fd, s->attrs.expr);
		place_comment(fd, ";");
	} else if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++) {
			generate_statement(fd, s->attrs.list.statements[x]);
		}
	} else if (s->kind==ret) {
		place_comment(fd, "return");
		place_comment(fd, "(");
		generate_expression(fd, s->attrs.expr);
		return_from_call(fd);
		place_comment(fd, ") ;");
	} else if (s->kind==declare) {
		return;
	} else if (s->kind==_if) {
		generate_if_statement(fd, s, retu, cond, block);
	} else if (s->kind==_while) {
		generate_while_loop(fd, s, retu, cond, block);
	} else if (s->kind==_break) {
		int *n=pop(loop_stack);
		char *jump_to;
		asprintf(&jump_to, "loop$end$%d", *n);
		place_comment(fd, "break;");
		jmp(fd, jump_to);
		free(jump_to);
		push(loop_stack, n);
	} else if (s->kind==_continue) {
		int *n=pop(loop_stack);
		char *jump_to;
		asprintf(&jump_to, "loop$start$%d", *n);
		place_comment(fd, "continue;");
		jmp(fd, jump_to);
		free(jump_to);
		push(loop_stack, n);
	} else if (s->kind==_goto) {
		place_comment(fd, "goto ");
		place_comment(fd, s->attrs.label_name);
		place_comment(fd, ";");

		jmp(fd, s->attrs.label_name);
	} else if (s->kind==label) {
		place_comment(fd, s->attrs.label_name);
		place_comment(fd, ":");

		place_label(fd, s->attrs.label_name);
	} else if (s->kind==_for) {
		generate_for_loop(fd, s, retu, cond, block);
	} else if (s->kind==do_while) {
		generate_do_while_loop(fd, s, retu, cond, block);
	} else if (s->kind==_switch) {
		generate_switch_statement(fd, s, retu, cond, block);
	}
	depth--;
}
