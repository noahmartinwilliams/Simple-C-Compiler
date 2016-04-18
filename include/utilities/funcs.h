#ifndef __UTILITIES_FUNCS_H__
#define __UTILITIES_FUNCS_H__
#include "types.h"

extern struct func_t* get_func_by_name(char *name);


extern void add_func(struct func_t *f);
extern void free_all_funcs();
extern void init_func(struct func_t *f);
extern void parser_handle_inline_func(int num_calls, struct func_t *f);
#endif
