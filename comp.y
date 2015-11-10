%{
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "printer.h"
#include "handle-types.h"
#include "handle-exprs.h"
#include "handle-statems.h"
#include "print-tree.h"
#include "handle-funcs.h"
#include "globals.h"
#include "generator.h"
#include "handle-vars.h"
#include "handle-registers.h"

extern int yydebug;
FILE *output;
%}
%union {
	long int l;
	struct expr_t *expr;
	struct statem_t *statem;
	struct type_t *type;
	char *str;
	struct func_t *func;
}
%token WHILE
%token RET
%token <l> CONST_INT
%token <str> IDENTIFIER
%type <expr> expression
%type <expr> binary_expr
%type <expr> assignable_expr
%type <statem> statement
%type <statem> statement_list
%type <statem> var_declaration
%type <type> type
%type <func> function
%%
file:  function {
	print_s($1);
	generate_function(output, $1);
	free_statem($1->statement_list);
	free_all_vars();
	free_all_types();
	free_all_registers();
};

function: type IDENTIFIER '(' ')' '{' statement_list '}' {
	struct func_t *f=malloc(sizeof(struct func_t));
	f->name=strdup($2);
	f->statement_list=$6;
	f->ret_type=$1;
	f->num_arguments=0;
	f->arguments=NULL;
	add_func(f);
	$$=f;
}
statement: expression ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=expr;
	s->attrs.expr=$1;
	$$=s;
} | '{' statement_list '}' {
	$$=$2;
} | var_declaration | WHILE '(' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_while;
	s->attrs._while.condition=$3;
	s->attrs._while.block=$5;
	$$=s;
} | RET expression ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=ret;
	s->attrs.expr=$2;
	$$=s;
};

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
} | assignable_expr ;

assignable_expr:  IDENTIFIER {
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
} | assignable_expr '=' expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	if ($1->type!=$3->type) {
		fprintf(stderr, "Type mismatch at line: %d character: %d\n", current_line, current_char);
		exit(1);
	}
	e->type=$3->type;
	e->kind=bin_op;
	e->left=$1;
	e->right=$3;
	e->attrs.bin_op=strdup("=");
	$$=e;

};

var_declaration: type IDENTIFIER ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=declare;
	s->attrs.var=malloc(sizeof(struct var_t));
	struct var_t *v=s->attrs.var;
	v->scope=1; /* TODO: get this working better later */
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
	output=fopen("output.s", "w+");
	/* TODO: finish testing addition expression. */
	setup_generator();
	yyparse();
	free_all_types();
	fclose(output);
	return 0;
}
