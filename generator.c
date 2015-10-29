#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include <string.h>

size_t word_size=4;
size_t int_size=4;
void setup_types()
{
	num_types++;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=malloc(sizeof(struct type_t));
	struct type_t *i=types[num_types-1];
	i->name=strdup("int");
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=int_size;
}
