#include "types.h"
#include <stdlib.h>

void free_expression(struct expr_t *e)
{
	if (e->kind==bin_op) {
		free(e->attrs.bin_op);
	}
	if (e->left!=NULL)
		free_expression(e);
	if (e->right!=NULL)
		free_expression(e);
	free(e);
}
