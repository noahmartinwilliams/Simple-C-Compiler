#include "handle-types.h"
#include "globals.h"
#include <stdlib.h>
#include "handle-exprs.h"

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
