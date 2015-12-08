#ifndef __JUMPS_H__
#define __JUMPS_H__
#include <stdio.h>

extern void jmp_ge(FILE *fd, char *name);
extern void place_label(FILE *fd, char *name);
extern void jmp_gt(FILE *fd, char *name);
extern void jmp_lt(FILE *fd, char *name);
extern void jmp_neq(FILE *fd, char *name);
extern void jmp_eq(FILE *fd, char *name);
extern void jmp(FILE *fd, char *name);

#endif
