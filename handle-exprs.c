#include <stdlib.h>
#include "types.h"

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
