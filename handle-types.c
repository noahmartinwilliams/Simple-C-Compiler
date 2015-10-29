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

void free_all_types()
{
	int x;
	for (x=0; x<num_types; x++) {
		free(types[x]->name);
		free(types[x]->body);
		free(types[x]);
	}
	free(types);
	types=NULL;
	num_types=0;
}
