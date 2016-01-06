#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "globals.h"
#include "print-tree.h"
#include "types.h"
#include "optimization-globals.h"

#ifdef DEBUG
void print_expr(char *pre, struct expr_t *e)
{
	if (e==NULL) {
		printf("(nil)");
		return;
	} 

	if (e->kind!=arg) {
		switch (e->kind) {
		case const_int:
			printf("%d", e->attrs.cint_val);
			break;
		case bin_op:
			printf("%s", e->attrs.bin_op);
			break;
		case var:
			printf("%s", e->attrs.var->name);
			break;
		case pre_un_op:
			printf("%s", e->attrs.un_op);
			break;
		case post_un_op:
			printf("%s", e->attrs.un_op);
			break;
		case funccall:
			printf("%s()", e->attrs.function->name);
			break;
		case const_str:
			printf("string literal: %s", e->attrs.cstr_val);
			break;
		}
	printf(", type: %s, type_size: %ld, pointer_depth: %ld\n", e->type->name, get_type_size(e->type), e->type->pointer_depth);
	}

	else if (e->kind==arg) {
		printf("argument: \n", pre);
		char *new_pre=NULL;
		asprintf(&new_pre, "%s |", pre);
		print_tree((__printer_function_t) print_expr, e->attrs.argument, new_pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
		free(new_pre);
	} 	
}
#endif

void free_expr(struct expr_t *e)
{
	if (e==NULL)
		return;

	switch (e->kind) {
	case bin_op:
		free(e->attrs.bin_op);
		break;

	case pre_un_op:
	case post_un_op:
		free(e->attrs.un_op);
		break;

	case const_str:
		free(e->attrs.cstr_val);
		break;

	case arg:
		free_expr(e->attrs.argument);
		break;

	case var:
		free_var(e->attrs.var);
		break;
	}

	if (e->left!=NULL)
		free_expr(e->left);

	if (e->right!=NULL)
		free_expr(e->right);

	free_type(e->type);
	free(e);
}

bool evaluate_constant_expr(char *op, struct expr_t *a, struct expr_t *b, struct expr_t **e2)
{
	struct expr_t *e=*e2;
	if (a->kind==const_int && b->kind==const_int && evaluate_constants) {
		e->kind=const_int;
		e->left=NULL;
		e->right=NULL;
		long int c=a->attrs.cint_val, d=b->attrs.cint_val, f=0;
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
	} else if (a->kind==const_int && b->kind!=const_int && a->attrs.cint_val==0 && optimize_dont_add_zero && !strcmp(op, "+")) {
		*e2=b;
		free_expr(a);
		return true;
	} else if (a->kind!=const_int && b->kind==const_int && b->attrs.cint_val==0 && optimize_dont_add_zero && !strcmp(op, "+")) {
		*e2=a;
		free_expr(b);
		return true;
	}
	return false;
}
