#ifndef __TYPES_H__
#define __TYPES_H__
#include <stdbool.h>
#include <stdlib.h>

typedef void (*__printer_function_t) (char*, void*); /* For type conversion for
the print_tree function so I don't get annoying warnings because I never pass it
a function pointer that has void* as the second argument type. */

struct tbody_t {
	size_t size;
};

struct type_t {
	char *name;
	int pointer_depth;
	struct tbody_t *body;
};
struct expr_t {
	enum { bin_op, pre_un_op, post_un_op, question, const_int, const_float, var } kind;
	struct expr_t *left, *middle, *right;
	struct type_t *type;
	union {
		struct var_t *var;
		long int cint_val;
		char *bin_op;
		char *un_op;
	} attrs;
};

struct var_t {
	struct type_t *type;
	off_t offset;
	int scope;
	char *name;
};

struct statem_t {
	enum { expr, list, declare, _while, ret, _if, _break, _continue } kind;
	union {
		struct expr_t *expr;
		struct var_t *var;
		struct {
			struct statem_t **statements;
			int num;
		} list;

		struct {
			struct expr_t *condition;
			struct statem_t *block;
		} _while;

		struct {
			struct expr_t *condition;
			struct statem_t *block;

			struct statem_t *else_block;
		} _if;
	} attrs;
};

struct func_t {
	char *name;
	struct statem_t *statement_list;
	struct type_t *ret_type;
	struct type_t **arguments;
	int num_arguments;
};

#endif
