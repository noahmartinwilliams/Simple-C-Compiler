#ifndef __JUMPS_H__
#define __JUMPS_H__
#include <stdio.h>
#include "types.h"

extern void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void compare_register_to_int(FILE *fd, struct reg_t *a, int i);
extern void jmp_ge(FILE *fd, char *name);
extern void place_label(FILE *fd, char *name);
extern void jmp_gt(FILE *fd, char *name);
extern void jmp_lt(FILE *fd, char *name);
extern void jmp_neq(FILE *fd, char *name);
extern void jmp_eq(FILE *fd, char *name);
extern void jmp(FILE *fd, char *name);
extern void jmp_le(FILE *fd, char *name);

#endif
