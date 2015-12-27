#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "globals.h"
#include "print-tree.h"
#include "types.h"

void print_expr(char *pre, struct expr_t *e)
{
	if (e->kind!=arg) {
		if (e->kind==const_int)
			printf("%d", e->attrs.cint_val);
		else if (e->kind==bin_op)
			printf("%s", e->attrs.bin_op);
		else if (e->kind==var)
			printf("%s", e->attrs.var->name);
		else if (e->kind==pre_un_op)
			printf("%s", e->attrs.un_op);
		else if (e->kind==post_un_op)
			printf("%s", e->attrs.un_op);
		else if (e->kind==funccall)
			printf("%s()", e->attrs.function->name);
		else if (e->kind==const_str)
			printf("string literal: %s", e->attrs.cstr_val);
	printf(", type: %s, type_size: %ld, pointer_depth: %ld\n", e->type->name, e->type->body->size, e->type->pointer_depth);
	}

	else if (e->kind==arg) {
		printf("argument: \n", pre);
		char *new_pre=NULL;
		asprintf(&new_pre, "%s |", pre);
		print_tree((__printer_function_t) print_expr, e->attrs.argument, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);
	} 	
}

void free_expr(struct expr_t *e)
{
	if (e->kind==bin_op && e->attrs.bin_op!=NULL) 
		free(e->attrs.bin_op);

	else if (e->kind==pre_un_op || e->kind==post_un_op)
		free(e->attrs.un_op);

	else if (e->kind==const_str)
		free(e->attrs.cstr_val);

	else if (e->kind==arg)
		free_expr(e->attrs.argument);

	else if (e->kind==var)
		free_var(e->attrs.var);

	if (e->left!=NULL)
		free_expr(e->left);

	if (e->right!=NULL)
		free_expr(e->right);

	free_type(e->type);
	free(e);
}

bool evaluate_constant_expr(char *op, struct expr_t *a, struct expr_t *b, struct expr_t *e)
{
	if (a->kind==const_int && b->kind==const_int && evaluate_constants) {
		e->kind=const_int;
		e->left=NULL;
		e->right=NULL;
		long int c=a->attrs.cint_val, d=a->attrs.cint_val, f=0;
		if (!strcmp(op, "+")) {
			f=c+d;
		} else if (!strcmp(op, "-")) {
			f=c-d;
		} else if (!strcmp(op, "/")) {
			f=c/d;
			/* NOTE: This might cause problems in the future if
			it's cross-compiling, and the target architecture
			handles integer rounding differently */
		} else if (!strcmp(op, "*")) {
			f=c*d;
		} else if (!strcmp(op, "==")) {
			f=c==d;
		} else if (!strcmp(op, "<")) {
			f=c<d;
		} else if (!strcmp(op, ">")) {
			f=c>d;
		} else if (!strcmp(op, "!=")) {
			f=c!=d;
		} else if (!strcmp(op, ">=")) {
			f=c>=d;
		} else if (!strcmp(op, "<=")) {
			f=c<=d;
		} else if (!strcmp(op, "<<")) {
			f=c<<d;
		} else if (!strcmp(op, ">>")) {
			f=c>>d;
		} else if (!strcmp(op, "|")) {
			f=c|d;
		} else if (!strcmp(op, "&")) {
			f=c&d;
		} else if (!strcmp(op, "^")) {
			f=c^d;
		} else if (!strcmp(op, "||")) {
			f=c||d;
		} else if (!strcmp(op, "&&")) {
			f=c&&d;
		}
		e->attrs.cint_val=f;
		free_expr(a);
		free_expr(b);
		return true;
	}
	return false;
}
