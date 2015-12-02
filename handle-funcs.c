#include "types.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>

void add_func(struct func_t *f)
{
	num_funcs++;
	funcs=realloc(funcs, num_funcs*sizeof(struct func_t*));
	funcs[num_funcs-1]=f;
}

struct func_t* get_func_by_name(char *name)
{
	int x;
	for (x=0; x<num_funcs; x++) {
		if (!strcmp(funcs[x]->name, name))
			return funcs[x];
	}
	return NULL;
}

void free_func(struct func_t *f)
{
	free(f->name);
	free_statem(f->statement_list);
	free(f->arguments);
	free(f);
}

void free_all_funcs(struct func_t *f)
{
	int x;
	for (x=0; x<num_funcs; x++) {
		free_func(funcs[x]);
	}
	free(funcs);
}
