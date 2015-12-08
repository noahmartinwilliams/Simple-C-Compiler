#ifndef __HANDLE_FUNCS_H__
#define __HANDLE_FUNCS_H__
#include "types.h"

extern struct func_t* get_func_by_name(char *name);


extern void add_func(struct func_t *f);
extern void free_all_funcs();
#endif
