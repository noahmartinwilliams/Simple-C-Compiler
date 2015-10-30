#include "types.h"
#include "globals.h"
#include <string.h>

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
