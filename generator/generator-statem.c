#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator-globals.h"
#include "generator-expr.h"
#include "generator-misc.h"
#include "globals.h"
#include "backend.h"
#include "generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"

void generate_statement(FILE *fd, struct statem_t *s)
{
	struct reg_t *retu=get_ret_register(word_size);
	if (s->kind==expr) {
		generate_expression(fd, s->attrs.expr);
	} else if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++) {
			generate_statement(fd, s->attrs.list.statements[x]);
		}
	} else if (s->kind==ret) {
		place_comment(fd, "return");
		place_comment(fd, "(");
		generate_expression(fd, s->attrs.expr);
		place_comment(fd, ")");
		fprintf(fd, "\tmovq %%rbp, %%rsp\n");
		fprintf(fd, "\tpopq %%rbp\n");
		if (!in_main)
			fprintf(fd, "\tret\n");
		else
			fprintf(fd, "\tmovq %%rax, %%rdi\n\tmovq $60, %%rax\n\tsyscall\n");
	} else if (s->kind==declare) {
		return;
	} else if (s->kind==_if) {
		place_comment(fd, "if (");
		generate_expression(fd, s->attrs._if.condition);
		compare_register_to_int(fd, retu, 0);

		unique_num++;
		char *unique_name_else, *unique_name_true;
		asprintf(&unique_name_else, "if$not$true$%d", unique_num);
		asprintf(&unique_name_true, "if$true$%d", unique_num);
		place_comment(fd, ")");

		jmp_eq(fd, unique_name_else);
		place_comment(fd, "{");
		generate_statement(fd, s->attrs._if.block);
		jmp(fd, unique_name_true);
		if (s->attrs._if.else_block==NULL) {
			place_comment(fd, "}");
			place_label(fd, unique_name_else);
		} else {
			place_label(fd, unique_name_else);
			place_comment(fd, "} else {");
			generate_statement(fd, s->attrs._if.else_block);
		}
		place_label(fd, unique_name_true);
		place_comment(fd, "}");
		free(unique_name_else);
		free(unique_name_true);
	} else if (s->kind==_while) {
		int *l=malloc(sizeof(int));
		unique_num++;
		*l=unique_num;
		push(loop_stack, l);

		char *loop_start, *loop_end;
		asprintf(&loop_start, "loop$start$%d", unique_num);
		asprintf(&loop_end, "loop$end$%d", unique_num);

		place_comment(fd, "while (");
		place_label(fd, loop_start);
		generate_expression(fd, s->attrs._while.condition);

		compare_register_to_int(fd, retu, 0);
		jmp_eq(fd, loop_end);
		place_comment(fd, ") {");

		generate_statement(fd, s->attrs._while.block);

		jmp(fd, loop_start);
		place_comment(fd, "}");
		place_label(fd, loop_end);

		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));

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
		place_comment(fd, "for (");
		generate_expression(fd, s->attrs._for.initial);
		place_comment(fd, "; ");

		int *l=malloc(sizeof(int));
		unique_num++;
		*l=unique_num;
		push(loop_stack, l);

		char *loop_start, *loop_end;
		asprintf(&loop_start, "loop$start$%d", unique_num);
		asprintf(&loop_end, "loop$end$%d", unique_num);

		place_label(fd, loop_start);
		generate_expression(fd, s->attrs._for.cond);

		compare_register_to_int(fd, retu, 0);
		jmp_eq(fd, loop_end);
		place_comment(fd, ") {");

		generate_statement(fd, s->attrs._for.block);

		generate_expression(fd, s->attrs._for.update);
		jmp(fd, loop_start);
		place_comment(fd, "}");
		place_label(fd, loop_end);

		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));

	} else if (s->kind==do_while) {
		int *l=malloc(sizeof(int));
		unique_num++;
		*l=unique_num;
		push(loop_stack, l);

		char *loop_start, *loop_end;
		asprintf(&loop_start, "loop$start$%d", unique_num);
		asprintf(&loop_end, "loop$end$%d", unique_num);

		place_comment(fd, "do {");

		place_label(fd, loop_start);
		generate_statement(fd, s->attrs.do_while.block);

		place_comment(fd, "} while (");
		generate_expression(fd, s->attrs.do_while.condition);
		compare_register_to_int(fd, retu, 0);
		jmp_neq(fd, loop_start);

		place_comment(fd, ") ;");
		place_label(fd, loop_end);
		free(loop_start);
		free(loop_end);
		free(pop(loop_stack));

	}
}
