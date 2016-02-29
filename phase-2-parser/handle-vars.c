#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include "handle-types.h"
#include <stdio.h>

void init_var(struct var_t *v)
{
	v->is_register=false;
	v->hidden=false;
	v->scope_depth=scope_depth;
}

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

	v->refcount--;
	if (v->refcount > 0)
		return;

	free_type(v->type);
	free(v->name);
	free(v);
}

struct expr_t* setup_var_expr(struct var_t *v)
{
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=var;
	e->attrs.var=v;
	v->refcount++;
	e->type=v->type;
	e->type->refcount++;

	e->left=e->right=NULL;
	return e;
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
		if (vars[x]!=NULL && !strcmp(name, vars[x]->name) && vars[x]->scope_depth<=scope_depth && vars[x]->scope_depth >= max_scope && !vars[x]->hidden) {
			highest_scope=vars[x];
			max_scope=highest_scope->scope_depth;
		}
	}
	return highest_scope;
}

void hide_current_scope()
{
	int x;
	for (x=0; x<num_vars; x++) {
		if (vars[x]!=NULL && vars[x]->scope_depth==scope_depth && vars[x]->hidden==false) {
			vars[x]->hidden=true;
		}
	}
}
