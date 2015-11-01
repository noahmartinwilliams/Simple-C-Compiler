#include <stdio.h>
#include "types.h"
#include "print-tree.h"
#include <stddef.h>

void print_expr(char *pre, struct expr_t *e)
{
	if (e->kind==const_int)
		printf("%d", e->attrs.cint_val);
	else if (e->kind==bin_op)
		printf("%s", e->attrs.bin_op);
	else if (e->kind==var)
		printf("%s", e->attrs.var->name);
	printf(", type: %s, type_size: %ld", e->type->name, e->type->body->size);
}

