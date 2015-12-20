%{
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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

void yyerror(const char *s);
extern int yydebug;
extern FILE* yyin;
FILE *output;

static inline struct expr_t* make_bin_op(char *X, struct expr_t *Y, struct expr_t *Z)
{
	/* TODO: Make sure that integers added to pointers get multiplied by the size of the pointer base type */
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

struct arguments_t {
	struct var_t **vars;
	int num_vars;
};
%}
%union {
	long int l;
	struct expr_t *expr;
	struct statem_t *statem;
	struct type_t *type;
	char *str;
	struct func_t *func;
	struct var_t *var;
	struct arguments_t *vars;
	char chr;
}
%define parse.error verbose
%token BREAK SHIFT_LEFT CONTINUE ELSE EQ_TEST IF NE_TEST 
%token RET STRUCT WHILE GE_TEST LE_TEST FOR
%token <str> STR_LITERAL 
%token SHIF_RIGHT EXTERN GOTO TEST_OR TEST_AND
%token <type> TYPE
%token <str> ASSIGN_OP
%token <l> CONST_INT
%token <str> IDENTIFIER
%token <chr> CHAR_LITERAL
%type <vars> arg_declaration
%type <expr> noncomma_expression
%type <expr> expression
%type <expr> binary_expr
%type <expr> assignable_expr
%type <expr> prefix_expr
%type <statem> statement
%type <statem> statement_list
%type <statem> var_declaration
%type <statem> var_declaration_list
%type <statem> var_declaration_ident
%type <type> type
%type <func> function
%type <statem> struct_var_declarations
%type <func> function_header
%type <expr> call_arg_list

%right '!'
%right '=' ASSIGN_OP
%left '*' '/'
%left '+' '-'
%left SHIFT_LEFT SHIFT_RIGHT
%left '<' LE_TEST '>' GE_TEST EQ_TEST NE_TEST
%left '&'
%left '^'
%left '|'
%left TEST_AND
%left TEST_OR
%nonassoc IFX
%nonassoc ELSE

%%
file: file_entry | file file_entry ;
file_entry:  function {
	if (print_trees)
		print_f($1);
	generate_function(output, $1);
	//free_all_vars();
	//free_all_types();
} | var_declaration {
	generate_global_vars(output, $1);
} | function_header ';';

arg_declaration: type var_declaration_ident {
	struct arguments_t *a=malloc(sizeof(struct arguments_t));
	a->vars=calloc(1, sizeof(struct var_t*));
	a->vars[0]=malloc(sizeof(struct var_t));
	a->vars[0]->scope=1;
	a->vars[0]->hidden=false;
	a->vars[0]->name=strdup($2->attrs.list.statements[0]->attrs.var->name);
	a->vars[0]->type=$1;
	a->num_vars=1;
	add_var(a->vars[0]);
	$$=a;
};

function_header: type IDENTIFIER '(' ')' {
	struct func_t *f=malloc(sizeof(struct func_t));
	f->name=$2;
	f->ret_type=$1;
	f->num_arguments=0;
	f->arguments=NULL;
	add_func(f);
	$$=f;
	current_function=$2;
} | type IDENTIFIER '(' arg_declaration ')' {
	struct func_t *f=malloc(sizeof(struct func_t));
	f->name=$2;
	f->ret_type=$1;
	f->num_arguments=0;
	f->arguments=$4->vars;
	f->num_arguments=$4->num_vars;
	free($4);
	add_func(f);
	$$=f;
	current_function=$2;
} | EXTERN function_header {
	$2->attributes|=_extern;
	$$=$2;
};
function: function_header '{' statement_list '}' {
	$1->statement_list=$3;
	$$=$1;
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
} | IF '(' expression ')' statement ELSE statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_if;
	s->attrs._if.condition=$3;
	s->attrs._if.block=$5;
	s->attrs._if.else_block=$7;
	$$=s;
} | IF '(' expression ')' statement %prec IFX{
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_if;
	s->attrs._if.condition=$3;
	s->attrs._if.block=$5;
	s->attrs._if.else_block=NULL;
	$$=s;
} | BREAK ';' { 
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_break;
	$$=s;
} | CONTINUE ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_continue;
	$$=s;
} | IDENTIFIER ':' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=label;
	s->attrs.label_name=strdup($1);
	free($1);
	$$=s;
} | GOTO IDENTIFIER ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_goto;
	s->attrs.label_name=strdup($2);
	free($2);
	$$=s;
} | FOR '(' expression ';' expression ';' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_for;
	s->attrs._for.initial=$3;
	s->attrs._for.cond=$5;
	s->attrs._for.update=$7;
	s->attrs._for.block=$9;
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

type: TYPE {
	if ($1==NULL) {
		yyerror("type not known\n");
		exit(1);
	}
	current_type=$1;
	$$=$1;
} | STRUCT IDENTIFIER '{' struct_var_declarations '}' {
	/* TODO: check to see if there's already a struct prototyped by this name, and use that instead if it exists. */
	struct type_t *type=malloc(sizeof(struct type_t));
	type->name=$2;
	type->pointer_depth=0;
	type->body=malloc(sizeof(struct tbody_t));

	struct tbody_t *body=type->body;
	body->attrs.vars.num_vars=$4->attrs.list.num;
	body->attrs.vars.vars=calloc(body->attrs.vars.num_vars, sizeof(struct var_t*));
	size_t size=0;
	int x;
	for (x=0; x<$4->attrs.list.num; x++) {
		size+=get_type_size($4->attrs.list.statements[x]->attrs.var->type);
		body->attrs.vars.vars[x]=$4->attrs.list.statements[x]->attrs.var;

	}

	/*TODO: free struct_var_declarations */

	body->size=size;
	body->is_struct=true;
	$$=type;

};

struct_var_declarations: type IDENTIFIER ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=list;
	s->attrs.list.num=1;
	s->attrs.list.statements=malloc(sizeof(struct statem_t*));
	s->attrs.list.statements[0]=malloc(sizeof(struct statem_t*));

	struct statem_t *var=s->attrs.list.statements[0];
	var->kind=declare;
	var->attrs.var=malloc(sizeof(struct var_t));
	struct var_t *v=var->attrs.var;

	v->type=$1;
	v->scope=scope;
	v->name=$2;
	$$=s;
} | struct_var_declarations type IDENTIFIER ';' {
	$1->attrs.list.num++;
	int num_statements=$1->attrs.list.num;
	struct statem_t **statements=$1->attrs.list.statements;
	$1->attrs.list.statements=realloc($1->attrs.list.statements, num_statements*sizeof(struct statem_t*));
	statements=$1->attrs.list.statements;

	statements[num_statements-1]=malloc(sizeof(struct statem_t));

	statements[num_statements-1]->kind=declare;
	statements[num_statements-1]->attrs.var=malloc(sizeof(struct var_t));

	struct var_t *v=statements[num_statements-1]->attrs.var;

	v->name=$3;
	v->type=$2;
	v->scope=scope;
	$$=$1;
};

call_arg_list: noncomma_expression {
	$$=$1;
};
expression: noncomma_expression ;
noncomma_expression: CONST_INT {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=get_type_by_name("int");
	e->kind=const_int;
	e->left=NULL;
	e->right=NULL;
	e->attrs.cint_val=$1;
	$$=e;
}  | binary_expr | '(' expression ')' {
	$$=$2;
} | assignable_expr | prefix_expr | IDENTIFIER '(' ')' {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=funccall;
	e->left=NULL;
	e->right=NULL;
	e->attrs.function=get_func_by_name($1);
	e->type=e->attrs.function->ret_type;
	free($1);
	$$=e;
} | IDENTIFIER '(' call_arg_list ')' {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=funccall;
	e->right=$3;
	e->left=NULL;
	e->attrs.function=get_func_by_name($1);
	e->type=e->attrs.function->ret_type;
	$$=e;
} | STR_LITERAL {
	/* TODO: add deallocation */
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=increase_type_depth(get_type_by_name("char"), 1);
	e->kind=const_str;
	e->right=NULL;
	e->left=NULL;
	e->attrs.cstr_val=generate_global_string(output, $1);
	$$=e;
};

prefix_expr: '&' assignable_expr {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("&");
	e->right=$2;
	e->left=NULL;
	e->type=increase_type_depth($2->type, 1);
	$$=e;
} | CHAR_LITERAL {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=const_int;
	e->type=get_type_by_name("char");
	e->left=NULL;
	e->right=NULL;
	e->attrs.cint_val=(long int) $1;
	$$=e;
} | '!' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("!");
	e->right=$2;
	e->left=NULL;
	e->type=$2->type;
	$$=e;
};

assignable_expr: IDENTIFIER {
	struct var_t *v=get_var_by_name($1);
	free($1);
	if (v==NULL) {
		yyerror("Unkown var");
		exit(1);
	}
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->left=NULL;
	e->right=NULL;
	e->kind=var;
	e->attrs.var=v;
	e->type=v->type;
	$$=e;
} | '*' assignable_expr {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("*");
	e->right=$2;
	e->left=NULL;
	e->type=decrease_type_depth($2->type, 1);
	$$=e;
};

binary_expr:  noncomma_expression '*' noncomma_expression {
	$$=make_bin_op("*", $1, $3);
} | noncomma_expression '/' noncomma_expression {
	$$=make_bin_op("/", $1, $3);
} | noncomma_expression '+' noncomma_expression {
	$$=make_bin_op("+", $1, $3);
} | noncomma_expression '-' noncomma_expression {
	$$=make_bin_op("-", $1, $3);
} | assignable_expr '=' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	struct expr_t *a=$1, *b=$3;
	parser_type_cmp(&a, &b);
	e->type=b->type;
	e->kind=bin_op;
	e->left=a;
	e->right=b;
	e->attrs.bin_op=strdup("=");
	$$=e;
} | noncomma_expression EQ_TEST noncomma_expression {
	$$=make_bin_op("==", $1, $3);
} | noncomma_expression '<' noncomma_expression {
	$$=make_bin_op("<", $1, $3);
} | noncomma_expression '>' noncomma_expression {
	$$=make_bin_op(">", $1, $3);
} | noncomma_expression NE_TEST noncomma_expression {
	$$=make_bin_op("!=", $1, $3);
} | assignable_expr ASSIGN_OP noncomma_expression {
	$$=make_bin_op($2, $1, $3);
} | noncomma_expression GE_TEST noncomma_expression {
	$$=make_bin_op(">=", $1, $3);
} | noncomma_expression LE_TEST noncomma_expression {
	$$=make_bin_op("<=", $1, $3);
} | noncomma_expression SHIFT_LEFT noncomma_expression {
	$$=make_bin_op("<<", $1, $3);
} | noncomma_expression SHIFT_RIGHT noncomma_expression {
	$$=make_bin_op(">>", $1, $3);
} | noncomma_expression '|' noncomma_expression {
	$$=make_bin_op("|", $1, $3);
} | noncomma_expression '&' noncomma_expression {
	$$=make_bin_op("&", $1, $3);
} | noncomma_expression '^' noncomma_expression {
	$$=make_bin_op("^", $1, $3);
} | noncomma_expression TEST_OR noncomma_expression {
	$$=make_bin_op("||", $1, $3);
} | noncomma_expression TEST_AND noncomma_expression {
	$$=make_bin_op("&&", $1, $3);
};

var_declaration_ident: IDENTIFIER { 
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=declare;

	struct var_t *v=malloc(sizeof(struct var_t));
	v->scope=scope;
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
	v->scope=scope; 
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
} | '*' IDENTIFIER {

	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=declare;

	struct var_t *v=malloc(sizeof(struct var_t));
	v->scope=scope; 
	v->name=$2;
	v->type=increase_type_depth(current_type, 1);
	add_var(v);
	s->attrs.var=v;

	struct statem_t *l=malloc(sizeof(struct statem_t));
	l->kind=list;
	l->attrs.list.num=1;
	l->attrs.list.statements=malloc(sizeof(struct statem_t*));
	l->attrs.list.statements[0]=s;

	$$=l;
};


var_declaration_list: var_declaration_ident {
	$$=$1;
} | var_declaration_list ',' var_declaration_ident {
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
void yyerror(const char *s)
{
	fprintf(stderr, "%s: In function '%s':\n", current_file, current_function);
	fprintf(stderr, "%s:%d:%d: %s\n", current_file, current_line, current_char, s);
	fprintf(stderr, "  ");
	char *no_indent=current_line_str;
	if (no_indent!=NULL)
		while ((*no_indent==' ' || *no_indent=='\t' ) && *no_indent!='\0')
			no_indent++;

	if (no_indent!=NULL) {
		fprintf(stderr, "%s\n", no_indent);
		fprintf(stderr, "  ");

		int x;
		for (x=1; x<current_char; x++) {
			fprintf(stderr, " ");
		}
		fprintf(stderr, "^\n");
	}
}

int main(int argc, char *argv[])
{
	if (argc<2) {
		fprintf(stderr, "Usage: output_file.s %s\n", argv[0]);
		exit(1);
	}
	current_file=argv[2];
	yyin=fopen(argv[2], "r");
	output=fopen(argv[1], "w+");
	setup_generator();
	yyparse();
	free_all_funcs();
	free_all_registers();
	free_all_types();
	free_all_vars();
	fclose(output);
	return 0;
}
