%{
#include <assert.h>
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
#include "handle-consts.h"
#include "handle-vars.h"
#include "generator/backend-exported.h"
#ifdef DEBUG
#include "printer.h"
#include "print-tree.h"
#endif
#include "types.h"

void yyerror(const char *s);
extern int yydebug;
extern FILE* yyin;
FILE *output;

static inline struct expr_t* make_bin_op(char *X, struct expr_t *Y, struct expr_t *Z)
{
	/* TODO: Make sure that integers added to pointers get multiplied by the size of the pointer base type */
	if (strlen(X)==2 && X[1]=='=' && X[0]!='='  && X[0]!='<' && X[0]!='>' && X[0]!='!' ) {
		struct expr_t *assignment=malloc(sizeof(struct expr_t));
		struct expr_t *or=malloc(sizeof(struct expr_t));
		assignment->kind=or->kind=bin_op;
		assignment->attrs.bin_op=strdup("=");
		assignment->type=Y->type;
		Y->type->refcount+=2;
		assignment->left=Y;
		assignment->right=or;

		or->attrs.bin_op=calloc(2, sizeof(char));
		or->attrs.bin_op[1]='\0';
		or->attrs.bin_op[0]=X[0];
		or->type=assignment->type;

		or->left=copy_expression(Y);
		or->right=Z;
		return assignment;
	}
	struct expr_t *e=malloc(sizeof(struct expr_t)); 
	struct expr_t *a=Y, *b=Z;
	parser_type_cmp(&a, &b);
	if (!is_test_op(X))
		e->type=b->type;
	else
		e->type=get_type_by_name("int", _normal);
	e->type->refcount++;
	if (!evaluate_constant_expr(X, a, b, &e)) {
		e->kind=bin_op;
		e->left=a;
		e->right=b;
		e->attrs.bin_op=strdup(X);
	}

	return e;
}

struct type_t *current_type=NULL;

struct arguments_t {
	struct var_t **vars;
	int num_vars;
};

static bool found_inline_in_function=false;

static inline bool is_constant_kind(struct expr_t *e)
{
	return e->kind==const_int || e->kind==const_float || e->kind==const_str;
}

struct enum_element {
	int i;
	char *name;
};

static inline struct expr_t* make_prefix_or_postfix_op(char *op, struct expr_t *e, struct type_t *t, bool is_prefix)
{
	if (is_constant_kind(e) && strcmp("-", op)) {
		/*TODO: make the error message easier to read for programmers
		who aren't experts on how compilers work. */
		yyerror("can not do prefix operator on constant expression.");
		exit (1);
	}

	struct expr_t *new=malloc(sizeof(struct expr_t));
	if (is_prefix) {
		new->kind=pre_un_op;
		new->right=e;
		new->left=NULL;
	} else {
		new->kind=post_un_op;
		new->left=e;
		new->right=NULL;
	}
	new->attrs.un_op=strdup(op);
	new->type=t;
	new->has_gotos=e->has_gotos;
	t->refcount++;
	return new;
}

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
	struct enum_element **enum_elements;
	struct enum_element *enum_element;
}
%define parse.error verbose
%token BREAK SHIFT_LEFT CONTINUE ELSE EQ_TEST IF NE_TEST RETURN STRUCT WHILE GE_TEST LE_TEST FOR INC_OP DO
%token SHIFT_RIGHT EXTERN GOTO TEST_OR TEST_AND DEC_OP TYPEDEF MULTI_ARGS STATIC INLINE SIZEOF
%token POINTER_OP DEFAULT SWITCH CASE ALIGNOF ENUM LONG UNSIGNED SIGNED SHORT
%token <str> STR_LITERAL 
%token <type> TYPE
%token <str> ASSIGN_OP
%token <l> CONST_INT
%token <str> IDENTIFIER
%token <chr> CHAR_LITERAL
%token UNION REGISTER CONST
%type <vars> arg_declaration
%type <enum_elements> enum_elements
%type <enum_element> enum_element
%type <expr> noncomma_expression expression binary_expr assignable_expr prefix_expr call_arg_list postfix_expr
%type <expr> maybe_empty_expr
%type <statem> statement statement_list var_declaration struct_var_declarations
%type <type> type 
%type <type> type_with_stars
%type <func> function function_header
%type <statem> var_declaration_start
%type <statem> switch_element switch_list
%type <l> stars

%right '=' ASSIGN_OP
%right '!' '~' INC_OP DEC_OP
%left '?' ':'
%left TEST_OR
%left TEST_AND
%left '|'
%left '^'
%left '<' LE_TEST '>' GE_TEST EQ_TEST NE_TEST
%left '&'
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%left '(' ')' '.' POINTER_OP
%left ','
%nonassoc IFX
%nonassoc ELSE

%%
file: file_entry | file file_entry ;
file_entry:  function {
	add_func($1);
	#ifdef DEBUG
	if (print_trees)
		print_f($1);
	#endif
	if (!$1->do_inline)
		generate_function(output, $1);
} | var_declaration {
	if ($1!=NULL)
		generate_global_vars(output, $1);
} | function_header ';' {
	add_func($1);
	multiple_functions=true;
} | TYPEDEF type_with_stars IDENTIFIER ';' {
	struct type_t *t=malloc(sizeof(struct type_t));
	memcpy(t, $2, sizeof(struct type_t));
	t->refcount=1;
	t->native_type=true;
	t->name=strdup($3);
	t->body->refcount++;
	free($3);
	add_type(t);
} | type ';' ;


include(functions.m4)
include(statements.m4)
include(expressions.m4)
include(variables.m4)
include(types.m4)

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
	exit (1);
}

