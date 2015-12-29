#ifndef __HANDLE_TYPES_H__
#define __HANDLE_TYPES_H__
#include "types.h"

extern struct type_t* get_type_by_name(char *name);
extern void free_type(struct type_t *t);
extern void free_tbody(struct tbody_t *t);
extern int get_type_index_by_name(char *name);
extern void parser_type_cmp(struct expr_t **a, struct expr_t **b);
extern struct type_t* increase_type_depth(struct type_t *t, int n);
extern struct type_t* decrease_type_depth(struct type_t *t, int n);
extern size_t get_type_size(struct type_t *t);
extern void add_type(struct type_t *t);
extern struct type_t* get_struct_or_union_attr_type(struct type_t *t, char *name);
extern void free_native_type(struct type_t *t);

extern off_t get_offset_of_member(struct type_t *t, char *name);
extern struct var_t* get_var_member(struct type_t *t, char *name);
#endif
