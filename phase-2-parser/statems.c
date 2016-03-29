#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#ifdef DEBUG
#include "print-tree.h"
#endif
#include "parser/types.h"
#include "globals.h"
#include "parser/exprs.h"

void free_statem(struct statem_t *s)
{
	if (s==NULL)
		return;

	if (s->expr!=NULL)
		free_expr(s->expr);

	if (s->right!=NULL)
		free_statem(s->right);

	if (s->left!=NULL)
		free_statem(s->left);

	switch(s->kind) {
	case label:
	case _goto:
		free(s->attrs.label_name);
		break;

	case _for:
		if (s->attrs._for.initial!=NULL)
			free_expr(s->attrs._for.initial);

		if (s->attrs._for.update!=NULL)
			free_expr(s->attrs._for.update);
		break;

	case declare:
		free_var(s->attrs.var);
	}
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
	if (s==NULL) {
		fprintf(stderr, "Error: NULL s passed to print_statem.\n");
		exit(1);
	}
	char *new_pre;
	int x=0;

	if (s->expr!=NULL)
		x++;
	if (s->left!=NULL)
		x++;
	if (s->right!=NULL)
		x++;

	switch(s->kind) {
	case expr:
		fprintf(stderr, "%s|_statement kind: expression\n", pre);
		break;
	case block:
		fprintf(stderr, "%s|_statement kind: block\n", pre);
		break;
	case declare: 
		fprintf(stderr, "%s|_statment kind: declare, var: %s, type: %s, pointer_depth: %d, size: %ld \n", pre, s->attrs.var->name, s->attrs.var->type->name, s->attrs.var->type->num_arrays, get_type_size(s->attrs.var->type));
		break;
	case _while:
		fprintf(stderr, "%s|_statement kind: while loop\n", pre);
		break;
	case ret:
		fprintf(stderr, "%s|_statement kind: return\n", pre);
		break;
	case _if:
		fprintf(stderr, "%s|_statement kind: if statement\n", pre);
		break;
	case _break:
		fprintf(stderr, "%s|_ break\n", pre);
		break;
	case _continue:
		fprintf(stderr, "%s|_ continue\n", pre);
		break;
	case _goto:
		fprintf(stderr, "%s|_ goto %s\n", pre, s->attrs.label_name);
		break;
	case label:
		fprintf(stderr, "%s|_ label %s:\n", pre, s->attrs.label_name);
		break;
	case _for:
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
		return;
	case do_while :
		fprintf(stderr, "%s|_statement kind: do while loop\n", pre);
		break;
	case _switch:
		fprintf(stderr, "%s|_ switch\n", pre);
		break;
	case _case:
		fprintf(stderr, "%s|_ statement kind: case \n", pre);
		break;
	}

	new_pre=fill_with_branches(pre, x);
	if (s->expr!=NULL) {
		print_e2(new_pre, s->expr);
		free(new_pre);
		x--;
		new_pre=fill_with_branches(pre, x);
	}

	if (s->left!=NULL) {
		print_statem(new_pre, s->left);
		free(new_pre);
		x--;
		new_pre=fill_with_branches(pre, x);
	}

	if (s->right!=NULL) {
		print_statem(new_pre, s->right);
	}

	free(new_pre);
}
#endif

void init_statem(struct statem_t *s)
{
	s->expr=NULL;
	s->left=s->right=NULL;
	s->has_gotos=false;
}

