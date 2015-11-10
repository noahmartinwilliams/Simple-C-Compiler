#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "print-tree.h"
#include "types.h"

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

void free_expr(struct expr_t *e)
{
	if (e->kind==bin_op) {
		free(e->attrs.bin_op);
	}
	if (e->left!=NULL)
		free_expr(e->left);
	if (e->right!=NULL)
		free_expr(e->right);
	free(e);
}
