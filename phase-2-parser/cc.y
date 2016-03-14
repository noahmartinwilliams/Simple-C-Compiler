%{
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "generator/generator.h"
#include "globals.h"
#include "parser/exprs.h"
#include "parser/funcs.h"
#include "parser/statems.h"
#include "parser/types.h"
#include "parser/consts.h"
#include "parser/vars.h"
#ifdef DEBUG
#include "printer.h"
#include "print-tree.h"
#endif
#include "types.h"

void yyerror(const char *s);
extern int yydebug;
extern FILE* yyin;
FILE *output;


struct type_t *current_type=NULL;

struct arguments_t {
	struct var_t **vars;
	int num_vars;
};

static bool found_inline_in_function=false;


struct enum_element {
	int i;
	char *name;
};

static inline struct expr_t* postfix_expr(char *op, struct expr_t *e, struct type_t *t)
{
	return prefix_or_postfix_expr(op, e, t, false);
}

static inline struct expr_t* prefix_expr(char *op, struct expr_t *e, struct type_t *t)
{
	return prefix_or_postfix_expr(op, e, t, true);
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
	struct {
		size_t *dimensions;
		int num_dimensions;
		char *name;
	} declare_ident;
}
%define parse.error verbose
%token BREAK SHIFT_LEFT CONTINUE ELSE EQ_TEST IF NE_TEST RETURN STRUCT WHILE GE_TEST LE_TEST FOR INC_OP DO
%token SHIFT_RIGHT EXTERN GOTO TEST_OR TEST_AND DEC_OP TYPEDEF MULTI_ARGS STATIC INLINE SIZEOF
%token POINTER_OP DEFAULT SWITCH CASE ALIGNOF ENUM LONG UNSIGNED SIGNED SHORT
%token UNION REGISTER CONST
%token <str> STR_LITERAL 
%token <type> TYPE
%token <str> ASSIGN_OP
%token <l> CONST_INT
%token <str> IDENTIFIER
%token <chr> CHAR_LITERAL
%type <str> possibly_blank_ident
%type <vars> arg_declaration
%type <declare_ident> declared_ident
%type <enum_elements> enum_elements
%type <enum_element> enum_element
%type <expr> noncomma_expression expression binary_expr assignable_expr prefix_expr call_arg_list postfix_expr
%type <expr> possibly_blank_expr possibly_blank_assignment
%type <statem> statement block var_declaration struct_var_declarations
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
%left '(' ')'  '.' POINTER_OP '[' ']'
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

