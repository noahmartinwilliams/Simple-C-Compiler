#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include "handle-types.h"

void add_var(struct var_t *v)
{
	num_vars++;
	vars=realloc(vars, num_vars*sizeof(struct var_t*));
	vars[num_vars-1]=v;
}

void free_var(struct var_t *v)
{
	int i=get_type_index_by_name(v->type->name);
	if (i!=-1) {
		types[i]=NULL;
		free_type(v->type);
	}

	free(v->name);
	free(v);
}

void free_all_vars()
{
	int x;
	for (x=0; x<num_vars; x++) {
		free_var(vars[x]);
	}
	free(vars);
}

struct var_t* get_var_by_name(char *name)
{
	int x;
	for (x=0; x<num_vars; x++) {
		if (!strcmp(name, vars[x]->name))
			return vars[x];
	}
	return NULL;
}
