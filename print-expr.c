#include <stdio.h>
#include "types.h"

char* print_expr(char *pre, struct expr_t *e)
{
	if (e->kind==const_int)
		printf("%d", e->attrs.cint_val);
}
