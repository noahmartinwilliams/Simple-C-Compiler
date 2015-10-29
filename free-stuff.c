#include "types.h"
#include <stdlib.h>

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

void free_statem(struct statem_t *s)
{
	if (s->kind==expr) {
		free_expr(s->attrs.expr);
	} else if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++) {
			free_statem(s->attrs.list.statements[x]);
		}
		free(s->attrs.list.statements);
	}
	free(s);
}
