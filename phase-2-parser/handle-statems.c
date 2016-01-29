#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#ifdef DEBUG
#include "print-tree.h"
#include "printer.h"
#endif
#include "handle-types.h"
#include "globals.h"
#include "handle-exprs.h"
#include "handle-exprs.h"

void free_statem(struct statem_t *s)
{
	if (s==NULL)
		return;
	if (s->kind==expr || s->kind==ret) {
		free_expr(s->attrs.expr);
	} else if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++) {
			free_statem(s->attrs.list.statements[x]);
		}
		free(s->attrs.list.statements);
	} else if (s->kind==_while) {
		free_expr(s->attrs._while.condition);
		free_statem(s->attrs._while.block);
	} else if (s->kind==_if) {
		free_expr(s->attrs._if.condition);
		free_statem(s->attrs._if.block);
		if (s->attrs._if.else_block!=NULL)
			free_statem(s->attrs._if.else_block);
	} else if (s->kind==label || s->kind==_goto) {
		free(s->attrs.label_name);
	} else if (s->kind==_for) {
		struct expr_t *initial=s->attrs._for.initial;
		if (initial!=NULL)
			free_expr(initial);

		free_expr(s->attrs._for.update);
		free_statem(s->attrs._for.block);
	} else if (s->kind==do_while) {
		free_expr(s->attrs.do_while.condition);
		free_statem(s->attrs.do_while.block);
	} else if (s->kind==declare) 
		free_var(s->attrs.var);
	else if (s->kind==_case) {
		free_expr(s->attrs._case.condition);
		free_statem(s->attrs._case.block);
	} else if (s->kind==_switch) {
		free_statem(s->attrs._switch.cases);
		free_expr(s->attrs._switch.tester);
	} else if (s->kind==_default)
		free_statem(s->attrs._default.def);
	free(s);
}

#ifdef DEBUG
static inline char* fill_with_branches(char *pre, int num_branches)
{
	int x;
	char *new_pre;
	asprintf(&new_pre, "%s ", pre);
	for (x=1; x<num_branches; x++) {
		char *new_pre2;
		asprintf(&new_pre2, "%s|", new_pre);
		free(new_pre);
		new_pre=new_pre2;
	}
	return new_pre;
}

void print_statem(char *pre, struct statem_t *s)
{
	char *new_pre;
	if (s->kind==expr) {
		fprintf(stderr, "%s|_statement kind: expression\n", pre);
		asprintf(&new_pre, "%s ", pre);
		print_e2(new_pre, s->attrs.expr);
		free(new_pre);
	} else if (s->kind==list) {
		fprintf(stderr, "%s|_statement kind: block\n", pre);
		char *new_pre;
		int x;
		int num=s->attrs.list.num;
		new_pre=fill_with_branches(pre, num);

		for (x=0; x<num; x++) {
			char *tmp;
			print_statem(new_pre, s->attrs.list.statements[x]);
			free(new_pre);
			new_pre=fill_with_branches(pre, (num-x)-1);
		}
		free(new_pre);

	} else if (s->kind==declare) {
		fprintf(stderr, "%s|_statment kind: declare, var: %s, type: %s, pointer_depth: %d, size: %ld \n", pre, s->attrs.var->name, s->attrs.var->type->name, s->attrs.var->type->pointer_depth, get_type_size(s->attrs.var->type));
	} else if (s->kind==_while) {
		fprintf(stderr, "%s|_statement kind: while loop\n", pre);
		char *new_pre;
		asprintf(&new_pre, "%s |", pre);
		print_e2(new_pre, s->attrs._while.condition);
		free(new_pre);

		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->attrs._while.block);
		free(new_pre);
	} else if (s->kind==ret) {
		fprintf(stderr, "%s|_statement kind: return\n", pre);
		asprintf(&new_pre, "%s ", pre);
		print_e2(new_pre, s->attrs.expr);
		free(new_pre);
	} else if (s->kind==_if) {
		fprintf(stderr, "%s|_statement kind: if statement\n", pre);
		char *new_pre;
		if (s->attrs._if.else_block==NULL) {
			asprintf(&new_pre, "%s |", pre);
			print_e2(new_pre, s->attrs._if.condition);
			free(new_pre);
			asprintf(&new_pre, "%s ", pre);
			print_statem(new_pre, s->attrs._if.block);
			free(new_pre);
		} else {
			asprintf(&new_pre, "%s | | ", pre);
			print_e2(new_pre, s->attrs._if.condition);
			free(new_pre);

			asprintf(&new_pre, "%s | ", pre);
			print_statem(new_pre, s->attrs._if.block);
			free(new_pre);

			fprintf(stderr, "%s |_ else\n", pre);
			asprintf(&new_pre, "%s  ", pre);
			print_statem(new_pre, s->attrs._if.else_block);
			free(new_pre);
		} 
	} else if (s->kind==_break) {
		fprintf(stderr, "%s|_ break\n", pre);
	} else if (s->kind==_continue) {
		fprintf(stderr, "%s|_ continue\n", pre);
	} else if (s->kind==_goto) {
		fprintf(stderr, "%s|_ goto %s\n", pre, s->attrs.label_name);
	} else if (s->kind==label) {
		fprintf(stderr, "%s|_ label %s:\n", pre, s->attrs.label_name);
	} else if (s->kind==_for) {
		fprintf(stderr, "%s|_ for \n", pre);
		char *new_pre=NULL;
		asprintf(&new_pre, "%s | | | ", pre);
		print_e2(new_pre, s->attrs._for.initial);
		free(new_pre);
		
		asprintf(&new_pre, "%s | | ", pre);
		print_e2(new_pre, s->attrs._for.cond);

		free(new_pre);

		asprintf(&new_pre, "%s | ", pre);
		print_e2(new_pre, s->attrs._for.update);
		free(new_pre);

		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->attrs._for.block);
		free(new_pre);
	} else if (s->kind==do_while ) {
		fprintf(stderr, "%s|_statement kind: do while loop\n", pre);
		char *new_pre;
		asprintf(&new_pre, "%s |", pre);
		print_e2(new_pre, s->attrs.do_while.condition);
		free(new_pre);

		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->attrs.do_while.block);
		free(new_pre);
	} else if (s->kind==_switch) {
		fprintf(stderr, "%s|_ switch\n", pre);

		int x;
		int num=s->attrs._switch.cases->attrs.list.num;
		char *new_pre=fill_with_branches(pre, num+1);
		print_e2(new_pre, s->attrs._switch.tester);
		free(new_pre);

		new_pre=fill_with_branches(pre, num);

		struct statem_t **statements=s->attrs._switch.cases->attrs.list.statements;
		for (x=0; x<num; x++) {
			struct statem_t *statement=statements[x];
			char *new_pre2;
			if (statements[x]->kind==_case) {
				if (statement->attrs._case.condition!=NULL && statement->attrs._case.block!=NULL) {
					fprintf(stderr, "%s|_ case \n", new_pre);
					asprintf(&new_pre2, "%s |", new_pre);

					print_e2(new_pre2, statement->attrs._case.condition);
					free(new_pre2);
					asprintf(&new_pre2, "%s ", new_pre);
					print_statem(new_pre2, statement->attrs._case.block);
				} else if (statement->attrs._case.block==NULL && statement->attrs._case.condition!=NULL) {
					fprintf(stderr, "%s|_ case\n", new_pre);
					asprintf(&new_pre2, "%s ", new_pre);
					print_e2(new_pre2, statement->attrs._case.condition);
				} else 
					continue;
			} else {
				fprintf(stderr, "%s|_ default \n", new_pre);

				asprintf(&new_pre2, "%s ", new_pre);
				print_statem(new_pre2, statements[x]->attrs._default.def);
			}
			free(new_pre);
			free(new_pre2);

			new_pre=fill_with_branches(pre, (num-x)-1);
		}
		free(new_pre);
	}
}
#endif
