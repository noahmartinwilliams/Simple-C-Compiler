#ifndef __HANDLE_EXPRS_H__
#define __HANDLE_EXPRS_H__
#include "types.h"

extern void free_expr(struct expr_t *e);
extern void print_expr(char *pre, struct expr_t *e);


extern bool evaluate_constant_expr(char *op, struct expr_t *a, struct expr_t *b, struct expr_t *e);
#endif
