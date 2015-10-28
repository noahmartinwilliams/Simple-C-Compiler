%{
#include <stdio.h>
#include <stddef.h>
#include "types.h"
#include "print-expr.h"
#include "print-tree.h"
%}
%union {
	long int l;
}
%token <l> CONST_INT
%%
expression: CONST_INT {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=const_int;
	e->left=NULL;
	e->right=NULL;
	e->attrs.cint_val=$1;
	print_tree(&print_expr, e, "", offsetof(struct expr_t, left), offsetof(struct expr_t, right));
}
%%
void yyerror(char *s)
{
	printf("%s\n", s);
}

int main()
{
	yyparse();
	return 0;
}
