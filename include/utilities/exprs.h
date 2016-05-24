#ifndef __UTILITIES_EXPRS_H__
#define __UTILITIES_EXPRS_H__
#include "../types.h"
#include "print-tree.h"
#include <stdbool.h>

extern struct expr_t* var_expr(struct var_t *v);
extern struct expr_t* get_array_lower_size(struct expr_t *e);
extern struct expr_t* bin_expr(char *op, struct expr_t *left, struct expr_t *right, struct type_t *ret_type);
extern bool is_constant_kind(struct expr_t *e);
extern struct expr_t* prefix_or_postfix_expr(char *op, struct expr_t *e, struct type_t *t, bool is_prefix);
extern struct expr_t* convert_expr(struct expr_t *e, struct type_t *t);
extern struct expr_t* const_int_expr(size_t i, struct type_t *t);
extern struct expr_t* copy_expression(struct expr_t *e);
extern void free_expr(struct expr_t *e);
extern void print_expr(char *pre, struct expr_t *e);
extern bool evaluate_constant_expr(char *op, struct expr_t *a, struct expr_t *b, struct expr_t **e);
extern bool is_test_op(char *op);
extern struct expr_t* struct_dot_expr(struct expr_t *e, char *name);
extern struct expr_t* prefix_expr(char *op, struct expr_t *e, struct type_t *t);
static inline void print_e(struct expr_t *e)
{
	print_tree((__printer_function_t) &print_expr, e, "", offsetof(struct expr_t, left), offsetof(struct expr_t, right));
}

static inline void print_e2(char *pre, struct expr_t *e)
{
	print_tree((__printer_function_t) &print_expr, e, pre, offsetof(struct expr_t, left), offsetof(struct expr_t, right));
}

#endif
