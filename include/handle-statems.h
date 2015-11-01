#ifndef __HANDLE_STATEMS_H__
#define __HANDLE_STATEMS_H__
#include "types.h"

extern void free_statem(struct statem_t *s);

extern void print_statem(char *pre, struct statem_t *s);
#endif
