#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "print-tree.h"
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
	free(s);
}

void print_statem(char *pre, struct statem_t *s)
{
	char *new_pre;
	if (s->kind==expr) {
		printf("%s|_statement kind: expression\n", pre);
		asprintf(&new_pre, "%s ", pre);
		print_tree((__printer_function_t) &print_expr, s->attrs.expr, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);
	} else if (s->kind==list) {
		printf("%s|_statement kind: list\n", pre);
		char *new_pre=calloc(2*s->attrs.list.num, sizeof(char));
		int x;
		for (x=0; x<2*s->attrs.list.num; x+=2) {
			new_pre[x]=' ';
			new_pre[x+1]='|';
		}
		for (x=0; x<s->attrs.list.num; x++) {
			new_pre[2*s->attrs.list.num-2*x-1]='\0';
			char *tmp;
			asprintf(&tmp, "%s%s", pre, new_pre);
			print_statem(tmp, s->attrs.list.statements[x]);
			free(tmp);
		}

		free(new_pre);
	} else if (s->kind==declare) {
		printf("%s|_statment kind: declare, var: %s, type: %s, pointer_depth: %d, size: %ld \n", pre, s->attrs.var->name, s->attrs.var->type->name, s->attrs.var->type->pointer_depth, get_type_size(s->attrs.var->type));
	} else if (s->kind==_while) {
		printf("%s|_statement kind: while loop\n", pre);
		char *new_pre;
		asprintf(&new_pre, "%s |", pre);
		print_tree((__printer_function_t) &print_expr, s->attrs._while.condition, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);
		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->attrs._while.block);
		free(new_pre);
	} else if (s->kind==ret) {
		printf("%s|_statement kind: return\n", pre);
		asprintf(&new_pre, "%s ", pre);
		print_tree((__printer_function_t) &print_expr, s->attrs.expr, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);
	} else if (s->kind==_if) {
		printf("%s|_statement kind: if statement\n", pre);
		char *new_pre;
		if (s->attrs._if.else_block==NULL) {
			asprintf(&new_pre, "%s |", pre);
			print_tree((__printer_function_t) &print_expr, s->attrs._if.condition, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
			free(new_pre);
			asprintf(&new_pre, "%s ", pre);
			print_statem(new_pre, s->attrs._if.block);
			free(new_pre);
		} else {
			asprintf(&new_pre, "%s | | ", pre);
			print_tree((__printer_function_t) &print_expr, s->attrs._if.condition, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
			free(new_pre);

			asprintf(&new_pre, "%s | ", pre);
			print_statem(new_pre, s->attrs._if.block);
			free(new_pre);

			printf("%s |_ else\n", pre);
			asprintf(&new_pre, "%s  ", pre);
			print_statem(new_pre, s->attrs._if.else_block);
			free(new_pre);
		} 
	} else if (s->kind==_break) {
		printf("%s|_ break\n", pre);
	} else if (s->kind==_continue) {
		printf("%s|_ continue\n", pre);
	} else if (s->kind==_goto) {
		printf("%s|_ goto %s\n", pre, s->attrs.label_name);
	} else if (s->kind==label) {
		printf("%s|_ label %s:\n", pre, s->attrs.label_name);
	} else if (s->kind==_for) {
		printf("%s|_ for \n", pre);
		char *new_pre=NULL;
		asprintf(&new_pre, "%s | | | | ", pre);
		print_tree((__printer_function_t) &print_expr, s->attrs._for.initial, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);
		
		asprintf(&new_pre, "%s | | | ", pre);
		print_tree((__printer_function_t) &print_expr, s->attrs._for.cond, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));

		free(new_pre);

		asprintf(&new_pre, "%s | | ", pre);
		print_tree((__printer_function_t) &print_expr, s->attrs._for.update, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);

		asprintf(&new_pre, "%s | ", pre);
		print_statem(new_pre, s->attrs._for.block);
		free(new_pre);
	} else if (s->kind==do_while ) {
		printf("%s|_statement kind: do while loop\n", pre);
		char *new_pre;
		asprintf(&new_pre, "%s |", pre);
		print_tree((__printer_function_t) &print_expr, s->attrs.do_while.condition, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);
		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->attrs.do_while.block);
		free(new_pre);
	}
}
