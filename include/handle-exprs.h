#ifndef __HANDLE_EXPRS_H__
#define __HANDLE_EXPRS_H__
#include "types.h"
#include <stdbool.h>

extern struct expr_t* bin_expr(char *op, struct expr_t *left, struct expr_t *right);
extern bool is_constant_kind(struct expr_t *e);
extern struct expr_t* make_prefix_or_postfix_op(char *op, struct expr_t *e, struct type_t *t, bool is_prefix);
extern struct expr_t* convert_expr(struct expr_t *e, struct type_t *t);
extern struct expr_t* const_int_expr(int i, struct type_t *t);
extern struct expr_t* copy_expression(struct expr_t *e);
extern void free_expr(struct expr_t *e);
extern void print_expr(char *pre, struct expr_t *e);
extern bool evaluate_constant_expr(char *op, struct expr_t *a, struct expr_t *b, struct expr_t **e);
extern bool is_test_op(char *op);

#endif
