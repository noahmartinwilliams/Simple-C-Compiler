#ifndef __GENERATOR_BACKEND_FLOAT_ARITHMETIC_H__
#define __GENERATOR_BACKEND_FLOAT_ARITHMETIC_H__
#include <stdio.h>
#include "types.h"
#include "generator/types.h"

extern void assign_constant_float(FILE *fd, struct expr_t *e);
extern void float_add(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void float_sub(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void float_mul(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void float_div(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void compare_float_registers(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void free_float_register(FILE *fd, struct reg_t *reg);
extern struct reg_t* get_free_float_register(FILE *fd, size_t s, int depth);
extern struct reg_t* get_float_ret_register(size_t s);

#endif
