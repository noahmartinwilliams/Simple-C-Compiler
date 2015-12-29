#ifndef __CALLS_H__
#define __CALLS_H__
#include <stdio.h>
#include "types.h"

extern void start_call(FILE *fd, struct func_t *f);


extern void add_argument(FILE *fd, struct reg_t *reg, struct type_t *t );
extern void call(FILE *fd, struct func_t *f);
extern char* get_next_call_register(enum reg_use r);
extern void return_from_call(FILE *fd);
extern void make_function(FILE *fd, struct func_t *f);
#endif
