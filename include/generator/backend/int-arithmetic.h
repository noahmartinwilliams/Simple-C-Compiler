#ifndef __INT_ARITHMETIC_H__
#define __INT_ARITHMETIC_H__
#include "types.h"
#include "generator/generator-types.h"
#include <stdio.h>

extern void int_neg(FILE *fd, struct reg_t *r);
extern void int_inc(FILE *fd, struct reg_t *r);
extern void int_mul(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void int_div(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void int_sub(FILE *fd, struct reg_t *b, struct reg_t *a);
extern void int_add(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void int_dec(FILE *fd, struct reg_t *r);

#endif
