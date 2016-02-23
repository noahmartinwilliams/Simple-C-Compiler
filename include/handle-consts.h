#ifndef __HANDLE_CONSTS_H__
#define __HANDLE_CONSTS_H__
#include "types.h"

extern struct expr_t* is_constant(char *name);
extern void add_constant(char *name, int scope, struct expr_t *e);
extern void free_constant(struct const_t *c);
extern void free_all_constants();

#endif
