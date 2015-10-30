%{
#include <stdio.h>
#include <stddef.h>
#include "types.h"
#include "printer.h"
#include "handle-types.h"
#include "handle-exprs.h"
#include "handle-statems.h"
#include "globals.h"
#include "generator.h"
#include <string.h>
#include "handle-vars.h"

extern int yydebug;
%}
%union {
	long int l;
	struct expr_t *expr;
	struct statem_t *statem;
	struct type_t *type;
	char *str;
}
%token <l> CONST_INT
%token <str> IDENTIFIER
%type <expr> expression
%type <expr> binary_expr
%type <statem> statement
%type <statem> statement_list
%type <statem> var_declaration
%type <type> type
%%
file: statement {
	print_s($1);
	free_statem($1);
	free_all_vars();
	free_all_types();
};

statement: expression ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=expr;
	s->attrs.expr=$1;
	$$=s;
} | '{' statement_list '}' {
	$$=$2;
} | var_declaration ;

statement_list: statement { 
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=list;
	s->attrs.list.statements=calloc(1, sizeof(struct statem_t));
	s->attrs.list.statements[0]=$1;
	s->attrs.list.num=1;
	$$=s;
} | statement_list statement {
	$1->attrs.list.num++;
	$1->attrs.list.statements=realloc($1->attrs.list.statements, $1->attrs.list.num*sizeof(struct statem_t*));
	$1->attrs.list.statements[$1->attrs.list.num-1]=$2;
	$$=$1;
};

type: IDENTIFIER {
	struct type_t *t=get_type_by_name($1);
	free($1);
	if (t==NULL) {
		fprintf(stderr, "Error: line %d, char %d, type not known\n", current_line, current_char);
		exit(1);
	}
	$$=t;
};
expression: CONST_INT {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=get_type_by_name("int");
	e->kind=const_int;
	e->left=NULL;
	e->right=NULL;
	e->attrs.cint_val=$1;
	$$=e;
}  | binary_expr | '(' expression ')' {
	$$=$2;
} | IDENTIFIER {
	struct var_t *v=get_var_by_name($1);
	if (v==NULL) {
		fprintf(stderr, "Unknown var on line: %d, char: %d\n", current_line, current_char);
		exit(1);
	}
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=var;
	e->attrs.var=v;
	e->type=v->type;
	free($1);
	$$=e;
};
binary_expr:  expression '+' expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	/*TODO: handle type stuff */
	if ($1->type!=$3->type) {
		fprintf(stderr, "Type mismatch at line: %d character: %d\n", current_line, current_char);
		exit(1);
	}

	e->type=$1->type;
	if ($1->kind==const_int && $3->kind==const_int && evaluate_constants ) {
		e->kind=const_int;
		e->left=NULL;
		e->right=NULL;
		e->attrs.cint_val=$1->attrs.cint_val+$3->attrs.cint_val;
		free_expr($1);
		free_expr($3);
	} else {
		e->kind=bin_op;
		e->left=$1;
		e->right=$3;
		e->attrs.bin_op=strdup("+");
	}
	$$=e;
};

var_declaration: type IDENTIFIER ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=declare;
	s->attrs.var=malloc(sizeof(struct var_t));
	struct var_t *v=s->attrs.var;
	v->name=$2;
	v->type=$1;
	add_var(v);
	$$=s;
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
	free_all_types();
	return 0;
}
