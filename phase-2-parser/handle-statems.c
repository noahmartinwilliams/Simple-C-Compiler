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
		free_expr(s->expr);
	} else if (s->kind==_while) { /* TODO: free up block statement. */
		free_expr(s->expr);
		free_statem(s->right);
	} else if (s->kind==_if) {
		free_expr(s->expr);
		free_statem(s->left);
		if (s->right!=NULL)
			free_statem(s->right);
	} else if (s->kind==label || s->kind==_goto) {
		free(s->attrs.label_name);
	} else if (s->kind==_for) {
		struct expr_t *initial=s->attrs._for.initial;
		if (initial!=NULL)
			free_expr(initial);

		free_expr(s->attrs._for.update);
		free_statem(s->right);
	} else if (s->kind==do_while) {
		free_expr(s->expr);
		free_statem(s->right);
	} else if (s->kind==declare) {
		free_var(s->attrs._declare.var);
		if (s->expr!=NULL)
			free_expr(s->expr);
	} else if (s->kind==_case) {
		free_expr(s->expr);
		free_statem(s->right);
	} else if (s->kind==_switch) {
		free_statem(s->right);
		free_expr(s->expr);
	} else if (s->kind==_default)
		free_statem(s->right);
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

void init_statem(struct statem_t *s)
{
	s->expr=NULL;
	s->left=s->right=NULL;
}

void print_statem(char *pre, struct statem_t *s)
{
	if (s==NULL) {
		fprintf(stderr, "Error: NULL s passed to print_statem.\n");
		exit(1);
	}
	char *new_pre;
	if (s->kind==expr) {
		fprintf(stderr, "%s|_statement kind: expression\n", pre);
		asprintf(&new_pre, "%s ", pre);
		print_e2(new_pre, s->expr);
		free(new_pre);
	} else if (s->kind==list) {
		fprintf(stderr, "%s|_statement kind: block\n", pre);
		char *new_pre;
		if (s->right!=NULL)
			new_pre=fill_with_branches(pre, 2);
		else
			new_pre=fill_with_branches(pre, 1);

		print_statem(new_pre, s->left);
		free(new_pre);

		if (s->right!=NULL) {
			new_pre=fill_with_branches(pre, 1);
			print_statem(new_pre, s->right);
			free(new_pre);
		}
	} else if (s->kind==declare) {
		struct var_t *var=s->attrs._declare.var;
		if (s->expr!=NULL) {
			char *new_pre=NULL;
			new_pre=fill_with_branches(pre, 1);
			fprintf(stderr, "%s|_statment kind: declare, var: %s, type: %s, pointer_depth: %d, size: %ld \n", pre, var->name, var->type->name, var->type->pointer_depth, get_type_size(var->type));
			print_e2(new_pre, s->expr);
			free(new_pre);
		} else
			fprintf(stderr, "%s|_statment kind: declare, var: %s, type: %s, pointer_depth: %d, size: %ld \n", pre, var->name, var->type->name, var->type->pointer_depth, get_type_size(var->type));
	} else if (s->kind==_while) {
		fprintf(stderr, "%s|_statement kind: while loop\n", pre);
		char *new_pre;
		asprintf(&new_pre, "%s |", pre);
		print_e2(new_pre, s->expr);
		free(new_pre);

		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->right);
		free(new_pre);
	} else if (s->kind==ret) {
		fprintf(stderr, "%s|_statement kind: return\n", pre);
		asprintf(&new_pre, "%s ", pre);
		print_e2(new_pre, s->expr);
		free(new_pre);
	} else if (s->kind==_if) {
		fprintf(stderr, "%s|_statement kind: if statement\n", pre);
		char *new_pre;
		if (s->right==NULL) {
			asprintf(&new_pre, "%s |", pre);
			print_e2(new_pre, s->expr);
			free(new_pre);
			asprintf(&new_pre, "%s ", pre);
			print_statem(new_pre, s->left);
			free(new_pre);
		} else {
			asprintf(&new_pre, "%s | | ", pre);
			print_e2(new_pre, s->expr);
			free(new_pre);

			asprintf(&new_pre, "%s | ", pre);
			print_statem(new_pre, s->left);
			free(new_pre);

			fprintf(stderr, "%s |_ else\n", pre);
			asprintf(&new_pre, "%s  ", pre);
			print_statem(new_pre, s->right);
			free(new_pre);
		} 
	} else if (s->kind==_break)
		fprintf(stderr, "%s|_ break\n", pre);
	else if (s->kind==_continue)
		fprintf(stderr, "%s|_ continue\n", pre);
	else if (s->kind==_goto)
		fprintf(stderr, "%s|_ goto %s\n", pre, s->attrs.label_name);
	else if (s->kind==label)
		fprintf(stderr, "%s|_ label %s:\n", pre, s->attrs.label_name);
	else if (s->kind==_for) {
		fprintf(stderr, "%s|_ for \n", pre);
		char *new_pre=NULL;
		asprintf(&new_pre, "%s | | | ", pre);
		print_e2(new_pre, s->attrs._for.initial);
		free(new_pre);
		
		asprintf(&new_pre, "%s | | ", pre);
		print_e2(new_pre, s->expr);

		free(new_pre);

		asprintf(&new_pre, "%s | ", pre);
		print_e2(new_pre, s->attrs._for.update);
		free(new_pre);

		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->right);
		free(new_pre);
	} else if (s->kind==do_while ) {
		fprintf(stderr, "%s|_statement kind: do while loop\n", pre);
		char *new_pre;
		asprintf(&new_pre, "%s |", pre);
		print_e2(new_pre, s->expr);
		free(new_pre);

		asprintf(&new_pre, "%s ", pre);
		print_statem(new_pre, s->right);
		free(new_pre);
	} else if (s->kind==_switch) {
		fprintf(stderr, "%s|_ switch\n", pre);

		int x;
		new_pre=fill_with_branches(pre, 2);
		print_e2(new_pre, s->expr);
		free(new_pre);
		new_pre=fill_with_branches(pre, 1);

		print_statem(new_pre, s->right);
		free(new_pre);
	} else if (s->kind==_case) {
		fprintf(stderr, "%s|_ statement kind: case \n", pre);
		if (s->right!=NULL) {
			new_pre=fill_with_branches(pre, 2);
			print_e2(new_pre, s->expr);
			free(new_pre);
			new_pre=fill_with_branches(pre, 1);
			print_statem(new_pre, s->right);
			free(new_pre);
		} else {
			new_pre=fill_with_branches(pre, 1);
			print_e2(new_pre, s->expr);
			free(new_pre);
		}
	}
}
#endif
