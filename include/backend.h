#ifndef __BACKEND_H__
#define __BACKEND_H__
#include <stdlib.h>
#include "types.h"
#include "generator-types.h"

extern void free_all_registers();
extern void setup_registers();
extern struct reg_t* get_ret_register(size_t s);
extern void int_add(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void read_var(FILE *fd, struct var_t *v);
extern void expand_stack_space(FILE *fd, off_t off);
extern void free_register(FILE *fd, struct reg_t *r);

extern void int_sub(FILE *fd, struct reg_t *a, struct reg_t *b);
extern struct reg_t* get_free_register(FILE *fd, size_t s);
extern void place_label(FILE *fd, char *name);
extern void jmp_neq(FILE *fd, char *name);
extern void jmp_eq(FILE *fd, char *name);
extern void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void compare_register_to_int(FILE *fd, struct reg_t *a, int i);
extern void int_mul(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void jmp(FILE *fd, char *name);
#endif
