#ifndef __GENERATOR_BACKEND_FLOAT_ARITHMETIC_H__
#define __GENERATOR_BACKEND_FLOAT_ARITHMETIC_H__
#include <stdio.h>
#include "types.h"
#include "generator/generator-types.h"

extern void assign_constant_float(FILE *fd, struct expr_t *e);
extern void float_sub(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void compare_float_registers(FILE *fd, struct reg_t *a, struct reg_t *b);

#endif
