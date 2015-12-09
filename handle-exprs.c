#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "globals.h"
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
	else if (e->kind==pre_un_op)
		printf("%s", e->attrs.un_op);
	else if (e->kind==funccall)
		printf("%s()", e->attrs.function->name);
	else if (e->kind==arg)
		printf("argument: ");
	printf(", type: %s, type_size: %ld, pointer_depth: %ld", e->type->name, e->type->body->size, e->type->pointer_depth);
}

void free_expr(struct expr_t *e)
{
	if (e->kind==bin_op && e->attrs.bin_op!=NULL) 
		free(e->attrs.bin_op);
	else if (e->kind==pre_un_op || e->kind==post_un_op)
		free(e->attrs.un_op);

	if (e->left!=NULL)
		free_expr(e->left);

	if (e->right!=NULL)
		free_expr(e->right);

	free(e);
}

bool evaluate_constant_expr(char *op, struct expr_t *a, struct expr_t *b, struct expr_t *e)
{
	if (a->kind==const_int && b->kind==const_int && evaluate_constants) {
		e->kind=const_int;
		e->left=NULL;
		e->right=NULL;
		if (!strcmp(op, "+")) {
			e->attrs.cint_val=a->attrs.cint_val+b->attrs.cint_val;
		} else if (!strcmp(op, "-")) {
			e->attrs.cint_val=a->attrs.cint_val-b->attrs.cint_val;
		} else if (!strcmp(op, "/")) {
			e->attrs.cint_val=a->attrs.cint_val/b->attrs.cint_val;
			/* NOTE: This might cause problems in the future if
			it's cross-compiling, and the target architecture
			handles integer rounding differently */
		} else if (!strcmp(op, "*")) {
			e->attrs.cint_val=a->attrs.cint_val*b->attrs.cint_val;
		} else if (!strcmp(op, "==")) {
			e->attrs.cint_val=a->attrs.cint_val==b->attrs.cint_val;
		} else if (!strcmp(op, "<")) {
			e->attrs.cint_val=a->attrs.cint_val<b->attrs.cint_val;
		} else if (!strcmp(op, ">")) {
			e->attrs.cint_val=a->attrs.cint_val>b->attrs.cint_val;
		} else if (!strcmp(op, "!=")) {
			e->attrs.cint_val=a->attrs.cint_val!=b->attrs.cint_val;
		} else if (!strcmp(op, ">=")) {
			e->attrs.cint_val=a->attrs.cint_val>=b->attrs.cint_val;
		}
		free_expr(a);
		free_expr(b);
		return true;
	}
	return false;
}
