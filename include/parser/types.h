#ifndef __PARSER_TYPES_H__
#define __PARSER_TYPES_H__
#include <stdbool.h>
#include <stddef.h>
#include "../types.h"

extern struct type_t* add_array_dimensions(struct type_t *t, int num_dimensions, size_t *dimensions);
extern size_t get_array_size(struct type_t *t);
extern size_t get_deref_type_size(struct type_t *t);
extern bool is_complete_type(struct type_t *t);
extern void init_body(struct tbody_t *b);
extern void init_type(struct type_t *t);
extern bool is_signed(struct type_t *t);
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
extern size_t get_alignof(struct type_t *t);

extern off_t get_offset_of_member(struct type_t *t, char *name);
extern struct var_t* get_var_member(struct type_t *t, char *name);
extern struct type_t* get_type_by_name(char *name, enum type_kind kind);
extern struct type_t* copy_type(struct type_t *t);
extern struct tbody_t* copy_body(struct tbody_t *b);

static inline bool type_is_float(struct type_t *t)
{
	if (t->body==NULL) 
		return false;

	return t->body->core_type==_FLOAT;
}

static inline bool expr_is_float(struct expr_t *e)
{
	if (e==NULL)
		return false;

	return type_is_float(e->type);
}

#endif
