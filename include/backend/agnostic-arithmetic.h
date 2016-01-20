#ifndef __AGNOSTIC_ARITHMETIC_H__
#define __AGNOSTIC_ARITHMETIC_H__
#include "generator/generator-types.h"
#include <stdio.h>

extern void shift_left (FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void shift_right (FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void or(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void and(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void xor(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void test_or(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void test_and(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void test_invert(FILE *fd, struct reg_t *r);
extern void invert(FILE *fd, struct reg_t *r);

#endif
