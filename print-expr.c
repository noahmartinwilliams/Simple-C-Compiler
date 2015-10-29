#include <stdio.h>
#include "types.h"

void print_expr(char *pre, struct expr_t *e)
{
	if (e->kind==const_int)
		printf("%d", e->attrs.cint_val);
	else if (e->kind==bin_op)
		printf("%s", e->attrs.bin_op);
	printf(", type: %s, type_size: %ld", e->type->name, e->type->body->size);
}
