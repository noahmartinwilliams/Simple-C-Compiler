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
