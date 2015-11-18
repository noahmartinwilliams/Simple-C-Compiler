#ifndef __HANDLE_VARS_H__
#define __HANDLE_VARS_H__


extern void free_var(struct var_t *v);


extern void add_var(struct var_t *v);
extern void free_all_vars();
extern struct var_t* get_var_by_name(char *name);
extern void hide_current_scope();
#endif
