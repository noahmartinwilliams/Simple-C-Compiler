#ifndef __BACKEND_H__
#define __BACKEND_H__
#include <stdlib.h>
#include "types.h"
#include "generator-types.h"
#include "registers.h"
#include "jumps.h"
#include "agnostic-arithmetic.h"
#include "statics.h"

extern void int_add(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void read_var(FILE *fd, struct var_t *v);
extern void expand_stack_space(FILE *fd, off_t off);

extern void int_sub(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void compare_register_to_int(FILE *fd, struct reg_t *a, int i);
extern void int_mul(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void assign_constant_int(FILE *fd, int e);
extern void int_inc_by(FILE *fd, struct reg_t *a, char *dest);
extern void inc_by_int(FILE *fd, int i, char *dest, size_t size);
extern void dereference(FILE *fd, struct reg_t *reg, size_t size);
extern void assign_dereference(FILE *fd, struct reg_t *assign_from, struct reg_t *assign_to);
extern void call(FILE *fd, struct func_t *f);
extern void start_call(FILE *fd, struct func_t *f);
extern void backend_make_global_var(FILE *fd, struct var_t *v);
#endif
