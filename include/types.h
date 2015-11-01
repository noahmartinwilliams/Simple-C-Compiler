#ifndef __TYPES_H__
#define __TYPES_H__
#include <stdbool.h>
#include <stdlib.h>

struct tbody_t {
	size_t size;
};

struct type_t {
	char *name;
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
	} attrs;
};

struct var_t {
	struct type_t *type;
	char *name;
};
struct statem_t {
	enum { expr, list, declare, _while } kind;
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
	} attrs;
};

#endif
