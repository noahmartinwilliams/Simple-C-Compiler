#ifndef __GENERATOR_BACKEND_FLOAT_ARITHMETIC_H__
#define __GENERATOR_BACKEND_FLOAT_ARITHMETIC_H__
#include <stdio.h>
#include "types.h"
#include "generator/generator-types.h"

extern void float_add(FILE *fd, struct reg_t *r1, struct reg_t *r2);
extern void assign_float_constant(FILE *fd, struct expr_t *e);
extern void float_neg(FILE *fd, struct reg_t *r);

#endif
