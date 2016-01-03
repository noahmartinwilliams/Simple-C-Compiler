%{
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "generator/generator.h"
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
	e->type->refcount++;
	if (!evaluate_constant_expr(X, a, b, &e)) {
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
%token BREAK SHIFT_LEFT CONTINUE ELSE EQ_TEST IF NE_TEST RETURN STRUCT WHILE GE_TEST LE_TEST FOR INC_OP DO
%token SHIFT_RIGHT EXTERN GOTO TEST_OR TEST_AND DEC_OP TYPEDEF MULTI_ARGS STATIC INLINE
%token <str> STR_LITERAL 
%token <type> TYPE
%token <str> ASSIGN_OP
%token <l> CONST_INT
%token <str> IDENTIFIER
%token <chr> CHAR_LITERAL
%token UNION
%type <vars> arg_declaration
%type <expr> noncomma_expression expression binary_expr assignable_expr prefix_expr call_arg_list postfix_expr
%type <statem> statement statement_list var_declaration var_declaration_list var_declaration_ident struct_var_declarations
%type <type> type 
%type <l> multiple_stars
%type <func> function function_header
%type <statem> for_loop

%right '=' ASSIGN_OP
%right '!' '~' INC_OP DEC_OP
%left TEST_OR
%left TEST_AND
%left '|'
%left '^'
%left '&'
%left '<' LE_TEST '>' GE_TEST EQ_TEST NE_TEST
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/'
%left '(' ')' '.'
%nonassoc IFX
%nonassoc ELSE

%%
file: file_entry | file file_entry ;
file_entry:  function {
	add_func($1);
	if (print_trees)
		print_f($1);
	if (!$1->do_inline)
		generate_function(output, $1);
} | var_declaration {
	generate_global_vars(output, $1);
} | function_header ';' {
	add_func($1);
	register int x;
	for (x=0; x<$1->num_arguments; x++) {
		free_var($1->arguments[x]);
	}
	free($1->arguments);
	$1->arguments=NULL;
	multiple_functions=true;
} | TYPEDEF type IDENTIFIER ';' {
	struct type_t *t=malloc(sizeof(struct type_t));
	memcpy(t, $2, sizeof(struct type_t));
	t->refcount=1;
	t->native_type=true;
	t->name=strdup($3);
	t->body->refcount++;
	free($3);
	add_type(t);
};

arg_declaration: type var_declaration_ident {
	struct arguments_t *a=malloc(sizeof(struct arguments_t));
	a->vars=calloc(1, sizeof(struct var_t*));
	a->vars[0]=$2->attrs.list.statements[0]->attrs.var;
	a->vars[0]->scope_depth=1;
	a->vars[0]->hidden=false;
	a->vars[0]->refcount+=2;
	a->num_vars=1;
	add_var(a->vars[0]);
	free_statem($2);
	$$=a;
} | arg_declaration ',' type var_declaration_ident {
	struct arguments_t *a=$1;
	a->num_vars++;
	a->vars=realloc(a->vars, a->num_vars*sizeof(struct var_t*));
	int n=a->num_vars-1;
	a->vars[n]=$4->attrs.list.statements[0]->attrs.var;
	a->vars[n]->scope_depth=1;
	a->vars[n]->hidden=false;
	a->vars[n]->type->refcount++;
	a->vars[n]->refcount+=2;
	add_var(a->vars[n]);
	free_statem($4);
	free_type($3);
	$$=a;
};

multiple_stars: '*' '*' {
	$$=2;
} | multiple_stars '*' {
	$$=$1+1;
}

function_header: type multiple_stars IDENTIFIER '(' ')' {
	struct func_t *f=malloc(sizeof(struct func_t));
	init_func(f);
	f->name=strdup($3);
	f->has_var_args=false;
	f->ret_type=increase_type_depth($1, $2);
	$1->refcount++;
	f->num_arguments=0;
	f->arguments=NULL;
	f->statement_list=NULL;
	free(current_function);
	current_function=strdup(f->name);
	free($3);
	$$=f;
} | type IDENTIFIER '(' ')' {
	struct func_t *f=malloc(sizeof(struct func_t));
	init_func(f);
	f->name=strdup($2);
	f->has_var_args=false;
	f->ret_type=$1;
	$1->refcount++;
	free_type(current_type);
	f->num_arguments=0;
	f->arguments=NULL;
	f->statement_list=NULL;
	free(current_function);
	current_function=strdup(f->name);
	free($2);
	$$=f;
}| type IDENTIFIER '(' arg_declaration ')' {
	struct func_t *f=malloc(sizeof(struct func_t));
	f->name=strdup($2);
	init_func(f);
	f->ret_type=$1;
	$1->refcount++;
	f->num_arguments=0;
	f->arguments=$4->vars;
	f->num_arguments=$4->num_vars;
	free($4);
	free(current_function);
	current_function=strdup($2);
	free($2);
	$$=f;
} | EXTERN function_header {
	$2->attributes|=_extern;
	$$=$2;
} | type IDENTIFIER '(' arg_declaration ',' MULTI_ARGS ')' {
	struct func_t *f=malloc(sizeof(struct func_t));
	init_func(f);
	f->name=strdup($2);
	f->ret_type=$1;
	$1->refcount++;
	f->num_arguments=0;
	f->arguments=$4->vars;
	f->num_arguments=$4->num_vars;
	f->has_var_args=true;
	f->statement_list=NULL;
	free($4);
	free(current_function);
	current_function=strdup($2);
	free($2);
	$$=f;
} | STATIC function_header {
	$2->attributes|=_static;
	$$=$2;
} | INLINE function_header {
	$2->attributes|=_inline;
	$$=$2;
};

function: function_header '{' statement_list '}' {
	$1->statement_list=$3;
	$$=$1;
}

include(for.m4)
include(statements.m4)
include(expressions.m4)

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
	free_type(current_type);
	current_type=$1;
	current_type->refcount+=2;
	$$=$1;
} | STRUCT IDENTIFIER '{' struct_var_declarations '}' {
	/* TODO: check to see if there's already a struct prototyped by this name, and use that instead if it exists. */
	struct type_t *type=malloc(sizeof(struct type_t));
	type->refcount=1;
	type->name=strdup($2);
	type->pointer_depth=0;
	type->body=malloc(sizeof(struct tbody_t));

	struct tbody_t *body=type->body;
	body->attrs.vars.num_vars=$4->attrs.list.num;
	body->attrs.vars.vars=calloc(body->attrs.vars.num_vars, sizeof(struct var_t*));
	size_t size=0;
	register int x;
	body->attrs.vars.alignment=word_size;
	struct statem_t **statements=$4->attrs.list.statements;
	size_t alignment=body->attrs.vars.alignment;
	for (x=0; x<$4->attrs.list.num; x++) {
		struct var_t *var=statements[x]->attrs.var;
		int s=get_type_size(var->type);
		if (s<alignment)
			size+=alignment;
		else
			size+=s;
		body->attrs.vars.vars[x]=var;
		var->refcount=1;

	}

	/*TODO: free struct_var_declarations */

	body->size=size;
	body->is_struct=true;
	body->is_union=false;
	add_type(type);
	$$=type;
	current_type=type;
	free($2);

} | UNION IDENTIFIER '{' struct_var_declarations '}' {
	struct type_t *type=malloc(sizeof(struct type_t));
	type->refcount=1;
	type->pointer_depth=0;
	type->name=strdup($2);
	type->body=malloc(sizeof(struct tbody_t));
	type->body->refcount=1;

	type->body->is_union=true;
	type->body->is_struct=false;

	type->body->attrs.vars.num_vars=0;
	type->body->attrs.vars.vars=NULL;
	register int x;
	size_t max_size=0;
	struct var_t **v=type->body->attrs.vars.vars;
	int num_vars=type->body->attrs.vars.num_vars;
	struct statem_t **statements=$4->attrs.list.statements;
	for (x=0; x<$4->attrs.list.num; x++) {
		size_t s=get_type_size($4->attrs.list.statements[x]->attrs.var->type);
		num_vars++;
		v=realloc(v, num_vars*sizeof(struct var_t*));
		v[num_vars-1]=malloc(sizeof(struct var_t));

		v[num_vars-1]=statements[x]->attrs.var;
		v[num_vars-1]->refcount=1;
		if (s>=max_size)
			max_size=s;
	}
	type->body->attrs.vars.num_vars=num_vars;
	type->body->attrs.vars.vars=v;

	type->body->size=max_size;
	current_type=type;
	current_type->refcount++;
	free($2);
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
	v->refcount=1;

	v->type=$1;
	$1->refcount++;
	v->refcount=1;
	v->scope_depth=scope_depth;
	v->name=strdup($2);
	free($2);
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

	v->name=strdup($3);
	v->type=$2;
	$2->refcount++;
	v->scope_depth=scope_depth;
	v->refcount=1;
	free($3);
	$$=$1;
};

call_arg_list: noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=arg;
	e->left=NULL;
	e->right=NULL;
	e->type=$1->type;
	e->type->refcount++;
	e->attrs.argument=$1;
	$$=e;
} | call_arg_list ',' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	struct expr_t *tmp=$1;
	for (; tmp->right!=NULL; tmp=tmp->right) {
	}
	e->kind=arg;
	e->type=$3->type;
	e->type->refcount++;
	e->right=NULL;
	e->left=NULL;
	e->attrs.argument=$3;
	tmp->right=e;
	$$=$1;
};

var_declaration_ident: IDENTIFIER { 
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=declare;

	struct var_t *v=malloc(sizeof(struct var_t));
	v->scope_depth=scope_depth;
	v->name=strdup($1);
	v->type=current_type;
	v->type->refcount++;
	v->refcount=2;
	add_var(v);
	s->attrs.var=v;

	struct statem_t *l=malloc(sizeof(struct statem_t));
	l->kind=list;
	l->attrs.list.num=1;
	l->attrs.list.statements=malloc(sizeof(struct statem_t*));
	l->attrs.list.statements[0]=s;

	free($1);
	$$=l;
} | IDENTIFIER '=' expression {
	struct var_t *v=malloc(sizeof(struct var_t));
	v->scope_depth=scope_depth; 
	v->refcount=3;
	v->name=strdup($1);
	v->type=current_type;
	v->type->refcount++;
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
	e->type->refcount++;

	e->left=malloc(sizeof(struct expr_t));

	struct expr_t *left=e->left;
	left->kind=var;
	left->attrs.var=v;
	left->left=NULL;
	left->right=NULL;
	left->type=current_type;
	left->type->refcount++;

	e->right=$3;

	struct statem_t *l=malloc(sizeof(struct statem_t));
	l->kind=list;
	l->attrs.list.num=2;
	l->attrs.list.statements=calloc(2, sizeof(struct statem_t*));
	l->attrs.list.statements[0]=declaration;
	l->attrs.list.statements[1]=expression;

	free($1);
	$$=l;
} | '*' IDENTIFIER {

	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=declare;

	struct var_t *v=malloc(sizeof(struct var_t));
	v->refcount=2;
	v->scope_depth=scope_depth; 
	v->name=strdup($2);
	v->type=increase_type_depth(current_type, 1);
	add_var(v);
	s->attrs.var=v;

	struct statem_t *l=malloc(sizeof(struct statem_t));
	l->kind=list;
	l->attrs.list.num=1;
	l->attrs.list.statements=malloc(sizeof(struct statem_t*));
	l->attrs.list.statements[0]=s;

	free($2);
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
	free_type(current_type);
	current_type=NULL;
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
		fprintf(stderr, "Usage: %s output_file.s input_file.c \n", argv[0]);
		exit(1);
	}
	current_file=argv[2];
	yyin=fopen(argv[2], "r");
	output=fopen(argv[1], "w+");
	setup_generator();
	yyparse();
	free(current_function);
	free_type(current_type);
	free_all_funcs();
	cleanup_backend();
	free_all_types();
	free_all_vars();
	fclose(output);
	return 0;
}
