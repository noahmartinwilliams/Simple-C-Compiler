#ifndef __TYPES_H__
#define __TYPES_H__
#include <stdbool.h>

struct expr_t {
	enum { bin_op, pre_un_op, post_un_op, question, const_int, const_float } kind;
	struct expr_t *left, *middle, *right;
	struct type_t* type;
	union {
		long int cint_val;
	} attrs;
};

#endif
