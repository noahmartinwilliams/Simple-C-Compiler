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
	printf(", type: %s, type_size: %ld", e->type->name, e->type->body->size);
}

void print_statem(char *pre, struct statem_t *s)
{
	if (s->kind==expr) {
		printf("kind: expression\n");
		print_tree(print_expr, s->attrs.expr, pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
	}
}
