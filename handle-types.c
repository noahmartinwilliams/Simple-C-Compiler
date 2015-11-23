#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "types.h"

struct type_t* get_type_by_name(char *name)
{
	int x;
	for (x=0; x<num_types; x++) {
		if (!strcmp(name, types[x]->name))
			return types[x];
	}
	return NULL;
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
	if ((*a)->type!=(*b)->type) {
		fprintf(stderr, "Type mismatch at line: %d character: %d\n", current_line, current_char);
		exit(1);
	}
}

struct type_t* increase_type_depth(struct type_t *t, int n)
{
	struct type_t *new=malloc(sizeof(struct type_t));
	memcpy(new, t, sizeof(struct type_t));
	new->pointer_depth+=n;
	num_types++;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=new;
	return new;
}
