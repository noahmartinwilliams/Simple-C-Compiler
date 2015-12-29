#ifndef __GENERATOR_EXPR_H__
#define __GENERATOR_EXPR_H__
#include "types.h"
#include <stdio.h>

extern void generate_expression(FILE *fd, struct expr_t *e);
extern void generate_pre_unary_expression(FILE *fd, struct expr_t *e);
extern void generate_binary_expression(FILE *fd, struct expr_t *e);
#endif
