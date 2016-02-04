#ifndef __VARIABLES_H__
#define __VARIABLES_H__
#include "generator/types.h"
#include "types.h"
#include <stdio.h>

extern void backend_make_global_var(FILE *fd, struct var_t *v);
extern void read_var(FILE *fd, struct var_t *v);
extern void dereference(FILE *fd, struct reg_t *reg, size_t size);
extern void get_address(FILE *fd, struct expr_t *var);
extern void assign_dereference(FILE *fd, struct reg_t *assign_from, struct reg_t *assign_to);
extern void assign_var(FILE *fd, struct reg_t *src, struct var_t *dest);

#endif
