#ifndef __UTILITIES_VARS_H__
#define __UTILITIES_VARS_H__
#include "types.h"

extern void free_var(struct var_t *v);
extern void init_var(struct var_t *v);
extern void add_var(struct var_t *v);
extern void free_all_vars();
extern struct var_t* get_var_by_name(char *name);
extern void hide_current_scope();
extern struct expr_t* setup_var_expr(struct var_t *v);
#endif
