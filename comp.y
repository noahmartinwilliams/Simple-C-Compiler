%{
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "generator.h"
#include "globals.h"
#include "handle-exprs.h"
#include "handle-funcs.h"
#include "handle-statems.h"
#include "handle-types.h"
#include "handle-vars.h"
#include "printer.h"
#include "print-tree.h"
#include "types.h"

extern int yydebug;
FILE *output;

static inline struct expr_t* make_bin_op(char *X, struct expr_t *Y, struct expr_t *Z)
{
	struct expr_t *e=malloc(sizeof(struct expr_t)); 
	struct expr_t *a=Y, *b=Z;
	parser_type_cmp(&a, &b);
	e->type=a->type;
	if (!evaluate_constant_expr(X, a, b, e)) {
		e->kind=bin_op;
		e->left=a;
		e->right=b;
		e->attrs.bin_op=strdup(X);
	}
	return e;
}

static struct type_t *current_type=NULL;
%}
%union {
	long int l;
	struct expr_t *expr;
	struct statem_t *statem;
	struct type_t *type;
	char *str;
	struct func_t *func;
	struct var_v *var;
}
%token WHILE
%token RET
%token IF
%token ELSE
%token EQ_TEST
%token LT_TEST
%token GT_TEST
%token <l> CONST_INT
%token <str> IDENTIFIER
%type <expr> expression
%type <expr> binary_expr
%type <expr> assignable_expr
%type <statem> statement
%type <statem> statement_list
%type <statem> var_declaration
%type <statem> var_declaration_list
%type <statem> var_declaration_ident
%type <type> type
%type <func> function

%left '+' '-'
%left '*' '/'

%right '='
%%
file:  function {
	print_f($1);
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
	free($2);
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
} | IF '(' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_if;
	s->attrs._if.condition=$3;
	s->attrs._if.block=$5;
	s->attrs._if.else_block=NULL;
	$$=s;
} | IF '(' expression ')' statement ELSE statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_if;
	s->attrs._if.condition=$3;
	s->attrs._if.block=$5;
	s->attrs._if.else_block=$7;
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
	current_type=t;
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

binary_expr:  expression '*' expression {
	$$=make_bin_op("*", $1, $3);
} | expression '/' expression {
	$$=make_bin_op("/", $1, $3);
} | expression '+' expression {
	$$=make_bin_op("+", $1, $3);
} | expression '-' expression {
	$$=make_bin_op("-", $1, $3);
} | assignable_expr '=' expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	struct expr_t *a=$1, *b=$3;
	parser_type_cmp(&a, &b);
	e->type=b->type;
	e->kind=bin_op;
	e->left=a;
	e->right=b;
	e->attrs.bin_op=strdup("=");
	$$=e;
} | expression EQ_TEST expression {
	$$=make_bin_op("==", $1, $3);
} | expression LT_TEST expression {
	$$=make_bin_op("<", $1, $3);
} | expression GT_TEST expression {
	$$=make_bin_op(">", $1, $3);
};

var_declaration_ident: IDENTIFIER { 
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=declare;
	struct var_t *v=malloc(sizeof(struct var_t));
	v->scope=scope; /* TODO: get this working better later */
	v->name=$1;
	v->type=current_type;
	add_var(v);
	s->attrs.var=v;

	struct statem_t *l=malloc(sizeof(struct statem_t));
	l->kind=list;
	l->attrs.list.num=1;
	l->attrs.list.statements=malloc(sizeof(struct statem_t*));
	l->attrs.list.statements[0]=s;

	$$=l;
} | IDENTIFIER '=' expression {
	struct var_t *v=malloc(sizeof(struct var_t));
	v->scope=scope; /* TODO: get this working better later */
	v->name=$1;
	v->type=current_type;
	add_var(v);

	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	declaration->attrs.var=v;

	struct statem_t *expression=malloc(sizeof(struct statem_t));
	expression->kind=expr;
	expression->attrs.expr=malloc(sizeof(struct expr_t));

	struct expr_t *e=expression->attrs.expr;
	e->kind=bin_op;
	e->attrs.bin_op=strdup("=");
	e->type=current_type;

	e->left=malloc(sizeof(struct expr_t));
	e->left->kind=var;
	e->left->attrs.var=v;
	e->left->left=NULL;
	e->left->right=NULL;
	e->left->type=current_type;

	e->right=$3;

	struct statem_t *l=malloc(sizeof(struct statem_t));
	l->kind=list;
	l->attrs.list.num=2;
	l->attrs.list.statements=calloc(2, sizeof(struct statem_t*));
	l->attrs.list.statements[0]=declaration;
	l->attrs.list.statements[1]=expression;

	$$=l;
};


var_declaration_list: var_declaration_ident | var_declaration_list ',' var_declaration_ident {
	struct statem_t *s=$3;

	$1->attrs.list.num++;
	$1->attrs.list.statements=realloc($1->attrs.list.statements, $1->attrs.list.num*sizeof(struct statem_t*));
	$1->attrs.list.statements[$1->attrs.list.num-1]=s;
	$$=$1;
};
var_declaration: type var_declaration_list ';' {
	$$=$2;
};
%%
void yyerror(char *s)
{
	printf("%s on line: %d, char: %d\n", s, current_line, current_char);
}

int main(int argc, char *argv[])
{
	if (argc<2) {
		fprintf(stderr, "Usage: %s output_file.s\n", argv[0]);
		exit(1);
	}
	output=fopen(argv[1], "w+");
	setup_generator();
	yyparse();
	free_all_types();
	fclose(output);
	return 0;
}
