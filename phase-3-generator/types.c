#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/expr.h"
#include "globals.h"
#include "parser/types.h"
#include "generator/statem.h"

void setup_types()
{
	num_types=5;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=malloc(sizeof(struct type_t));
	struct type_t *i=types[num_types-1];
	struct tbody_t *b=NULL;

	i->name=strdup("int");
	init_type(i);
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=int_size;

	types[num_types-2]=malloc(sizeof(struct type_t));
	i=types[num_types-2];
	init_type(i);
	i->name=strdup("char");
	i->is_signed=false;
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=char_size;

	types[num_types-3]=malloc(sizeof(struct type_t));
	i=types[num_types-3];
	init_type(i);
	i->name=strdup("void");
	i->body=NULL;

	types[num_types-4]=malloc(sizeof(struct type_t));
	i=types[num_types-4];
	init_type(i);
	i->name=strdup("float");
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=float_size;
	b->core_type=_FLOAT;

	types[num_types-5]=malloc(sizeof(struct type_t));
	i=types[num_types-5];
	init_type(i);
	i->name=strdup("long");
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=long_size;
}

