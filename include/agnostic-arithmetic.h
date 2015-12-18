#ifndef __AGNOSTIC_ARITHMETIC_H__
#define __AGNOSTIC_ARITHMETIC_H__


extern void shift_left(FILE *fd, struct reg_t *src, struct reg_t *dest);


extern void shift_right(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void or(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void and(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void xor(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void test_or(FILE *fd, struct reg_t *a, struct reg_t *b);
#endif
