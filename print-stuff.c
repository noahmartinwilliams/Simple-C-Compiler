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
		printf("%s|_kind: expression\n", pre);
		print_tree(print_expr, s->attrs.expr, pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
	} else if (s->kind==list) {
		printf("%s|_kind: list\n", pre);
		char *new_pre=calloc(2*s->attrs.list.num, sizeof(char));
		int x;
		for (x=0; x<2*s->attrs.list.num; x+=2) {
			new_pre[x]=' ';
			new_pre[x+1]='|';
		}
		for (x=0; x<s->attrs.list.num; x++) {
			new_pre[2*s->attrs.list.num-2*x-1]='\0';
			print_statem(new_pre, s->attrs.list.statements[x]);
		}

		free(new_pre);
	} else if (s->kind==declare) {
		printf("%s|_kind: declare, var: %s, type: %s\n", pre, s->attrs.var->name, s->attrs.var->type->name);
	}
}
