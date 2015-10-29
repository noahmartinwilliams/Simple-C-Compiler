%{
#include <stdio.h>
#include <stddef.h>
#include "types.h"
#include "printer.h"
#include "handle-types.h"
#include "globals.h"
#include "generator.h"

extern int yydebug;
%}
%union {
	long int l;
	struct expr_t *expr;
}
%token <l> CONST_INT
%type <expr> expression
%type <expr> binary_expr
%%
file: expression {
	print_e($1);
};
expression: CONST_INT {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=get_type_by_name("int");
	e->kind=const_int;
	e->left=NULL;
	e->right=NULL;
	e->attrs.cint_val=$1;
	$$=e;
}  | binary_expr ;
binary_expr:  expression '+' expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	/*TODO: handle type stuff */
	if ($1->type!=$3->type) {
		fprintf(stderr, "Type mismatch at line: %d character: %d\n", current_line, current_char);
		exit(1);
	}
	e->type=$1->type;
	e->kind=bin_op;
	e->left=$1;
	e->right=$3;
	e->attrs.bin_op=strdup("+");
	$$=e;
};
%%
void yyerror(char *s)
{
	printf("%s\n", s);
}

int main()
{
	setup_types();
	yyparse();
	return 0;
}
