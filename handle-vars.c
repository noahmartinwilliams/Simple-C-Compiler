#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include "handle-types.h"

void add_var(struct var_t *v)
{
	num_vars++;
	vars=realloc(vars, num_vars*sizeof(struct var_t*));
	vars[num_vars-1]=v;
	v->hidden=false;
}

void free_var(struct var_t *v)
{
	if (v==NULL)
		return;

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
	struct var_t *highest_scope=NULL;
	int max_scope=0;
	for (x=0; x<num_vars; x++) {
		if (vars[x]!=NULL && !strcmp(name, vars[x]->name) && vars[x]->scope<=scope && vars[x]->scope > max_scope && !vars[x]->hidden) {
			highest_scope=vars[x];
			max_scope=highest_scope->scope;
		}
	}
	return highest_scope;
}

void hide_current_scope()
{
	int x;
	for (x=0; x<num_vars; x++) {
		if (vars[x]!=NULL && vars[x]->scope==scope && vars[x]->hidden==false) {
			vars[x]->hidden=true;
		}
	}
}
