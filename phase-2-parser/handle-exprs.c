#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "globals.h"
#include "print-tree.h"
#include "types.h"
#include "optimization-globals.h"

bool is_test_op(char *op)
{
	return !strcmp("<", op) || !strcmp(">", op) || !strcmp("<=", op) || !strcmp(">=", op) || !strcmp("!=", op) || !strcmp("==", op);
}

#ifdef DEBUG
void print_expr(char *pre, struct expr_t *e)
{
	if (e==NULL) {
		fprintf(stderr, "(nil)");
		return;
	} 

	if (e->kind!=arg) {
		switch (e->kind) {
		case const_int:
			fprintf(stderr, "%d", e->attrs.cint_val);
			break;
		case bin_op:
			fprintf(stderr, "%s", e->attrs.bin_op);
			break;
		case var:
			fprintf(stderr, "%s", e->attrs.var->name);
			break;
		case pre_un_op:
			fprintf(stderr, "%s", e->attrs.un_op);
			break;
		case post_un_op:
			fprintf(stderr, "%s", e->attrs.un_op);
			break;
		case funccall:
			fprintf(stderr, "%s()", e->attrs.function->name);
			break;
		case const_str:
			fprintf(stderr, "string literal: %s", e->attrs.cstr_val);
			break;
		case const_float:
			fprintf(stderr, "float: %s", e->attrs.cfloat);
		}
		if (e->type->body->core_type==_INT)
			fprintf(stderr, ", type: %s, type_size: %ld, pointer_depth: %ld, core_type: INT\n", e->type->name, get_type_size(e->type), e->type->pointer_depth);
		else
			fprintf(stderr, ", type: %s, type_size: %ld, pointer_depth: %ld, core_type: FLOAT\n", e->type->name, get_type_size(e->type), e->type->pointer_depth);
	}

	else if (e->kind==arg) {
		fprintf(stderr, "argument: \n", pre);
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

struct expr_t* copy_expression(struct expr_t *e)
{
	if (e==NULL)
		return NULL;

	struct expr_t *ret=malloc(sizeof(struct expr_t));
	memcpy(ret, e, sizeof(struct expr_t));
	ret->left=copy_expression(e->left);
	ret->right=copy_expression(e->right);

	ret->type->refcount++;

	switch (ret->kind) {
	case bin_op:
		ret->attrs.bin_op=strdup(e->attrs.bin_op);
		break;
	case pre_un_op:
	case post_un_op:
		ret->attrs.un_op=strdup(e->attrs.un_op);
		break;

	case const_str:
		ret->attrs.cstr_val=strdup(e->attrs.cstr_val);
		break;

	case arg:
		ret->attrs.argument=copy_expression(e->attrs.argument);
		break;
	
	case var:
		ret->attrs.var->refcount++;
		break;
	case const_float:
		ret->attrs.cfloat=strdup(e->attrs.cfloat);
		break;

	}
	return ret;
}

bool evaluate_constant_expr(char *op, struct expr_t *a, struct expr_t *b, struct expr_t **e2)
{
	struct expr_t *e=*e2;
	if (a->kind==const_int && b->kind==const_int && evaluate_constants) {
		e->kind=const_int;
		e->left=NULL;
		e->right=NULL;
		long int c=a->attrs.cint_val, d=b->attrs.cint_val, f=0;
		if (!strcmp(op, "+"))
			f=c+d;
		else if (!strcmp(op, "-"))
			f=c-d;
		else if (!strcmp(op, "/")) 
			f=c/d;
			/* NOTE: This might cause problems in the future if
			it's cross-compiling, and the target architecture
			handles integer rounding differently */
		else if (!strcmp(op, "*"))
			f=c*d;
		else if (!strcmp(op, "=="))
			f=c==d;
		else if (!strcmp(op, "<"))
			f=c<d;
		else if (!strcmp(op, ">"))
			f=c>d;
		else if (!strcmp(op, "!="))
			f=c!=d;
		else if (!strcmp(op, ">="))
			f=c>=d;
		else if (!strcmp(op, "<="))
			f=c<=d;
		else if (!strcmp(op, "<<"))
			f=c<<d;
		else if (!strcmp(op, ">>"))
			f=c>>d;
		else if (!strcmp(op, "|"))
			f=c|d;
		else if (!strcmp(op, "&"))
			f=c&d;
		else if (!strcmp(op, "^"))
			f=c^d;
		else if (!strcmp(op, "||"))
			f=c||d;
		else if (!strcmp(op, "&&"))
			f=c&&d;
		else if (!strcmp(op, "%"))
			f=c%d;
	
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
