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
	if (f->arguments!=NULL) {
		int x;
		for (x=0; x<f->num_arguments; x++) {
			free_var(f->arguments[x]);
		}
		free(f->arguments);
	}
	free_type(f->ret_type);
	free(f);
}

void free_all_funcs()
{
	int x;
	for (x=0; x<num_funcs; x++) 
		free_func(funcs[x]);
	free(funcs);
}

void init_func(struct func_t *f)
{
	f->attributes=0;
	f->num_calls=0;
	f->has_var_args=false;
	f->do_inline=false;
}

void parser_handle_inline_func(int num_calls, struct func_t *f)
{
	if (num_calls < 2 && f->attributes & _inline) {
		/* TODO: figure out better optimizations. */
		f->do_inline=true;
	}
}
