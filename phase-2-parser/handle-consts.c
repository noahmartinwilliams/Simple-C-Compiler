#include "types.h"
#include "globals.h"
#include <string.h>
#include <stdbool.h>

void free_constant(struct const_t *c)
{
	free(c->name);
	free_expr(c->e);
	free(c);
}

void free_all_constants()
{
	int x;
	for (x=0; x<num_consts; x++) {
		free_constant(constants[x]);
	}
	free(constants);
	constants=NULL;
	num_consts=0;
}

struct expr_t* is_constant(char *name)
{
	int x;
	for (x=0; x<num_consts; x++)
		if (!strcmp(name, constants[x]->name))
			return constants[x]->e;
	return NULL;
}

void add_constant(char *name, int scope, struct expr_t *e)
{
	struct const_t *c=malloc(sizeof(struct const_t));
	c->name=name;
	c->e=e;
	c->is_hidden=false;
	c->scope=scope;
	num_consts++;
	constants=realloc(constants, num_consts*sizeof(struct const_t*));
	constants[num_consts-1]=c;
}
