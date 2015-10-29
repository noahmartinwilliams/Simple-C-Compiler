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
	enum { bin_op, pre_un_op, post_un_op, question, const_int, const_float } kind;
	struct expr_t *left, *middle, *right;
	struct type_t *type;
	union {
		long int cint_val;
		char *bin_op;
	} attrs;
};

struct statem_t {
	enum { expr } kind;
	union {
		struct expr_t *expr;
	} attrs;
};

#endif
