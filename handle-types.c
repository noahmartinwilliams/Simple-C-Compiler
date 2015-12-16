#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "generator-globals.h"
#include "types.h"

extern void yyerror(char *s);
struct type_t* get_type_by_name(char *name)
{
	int x;
	for (x=0; x<num_types; x++) {
		if (!strcmp(name, types[x]->name))
			return types[x];
	}
	return NULL;
}

size_t get_type_size(struct type_t *t)
{
	if (t==NULL || t->body==NULL) {
		fprintf(stderr, "Internal Error: Null pointer passed to get_type_size\n");
		exit(0);
	}
	if (t->pointer_depth>0)
		return pointer_size;
	else 
		return t->body->size;
}

int get_type_index_by_name(char *name)
{
	int x;
	for (x=0; x<num_types; x++) {
		if (types[x]!=NULL)
			if (!strcmp(name, types[x]->name))
				return x;
	}
	return -1;
}

void free_tbody(struct tbody_t *t)
{
	free(t);
}

void free_type(struct type_t *t)
{
	if (t==NULL)
		return;
	free(t->name);
	free_tbody(t->body);
	free(t);
}

void free_all_types()
{
	int x;
	for (x=0; x<num_types; x++) {
		free_type(types[x]);
	}
	free(types);
	types=NULL;
	num_types=0;
}

void parser_type_cmp(struct expr_t **a, struct expr_t **b)
{
	if (a==NULL || b==NULL) {
		yyerror("Internal error: Expression is NULL when passed to parser_type_cmp)");
		exit(1);
	}

	if ((*a)->type==NULL || (*b)->type==NULL) {
		yyerror("Internal error: Expression is missing type when passed to parser_type_cmp");
		exit(1);
	}
	if ((*a)->type->pointer_depth!=(*b)->type->pointer_depth) {
		yyerror("Type mismatch");
		exit(1);
	}
}

struct type_t* increase_type_depth(struct type_t *t, int n)
{
	struct type_t *new=malloc(sizeof(struct type_t));
	memcpy(new, t, sizeof(struct type_t));
	new->pointer_depth+=n;
	return new;
}

struct type_t* decrease_type_depth(struct type_t *t, int n)
{
	struct type_t *new=malloc(sizeof(struct type_t));
	memcpy(new, t, sizeof(struct type_t));
	new->pointer_depth-=n;
	return new;
}
