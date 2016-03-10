#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "generator/globals.h"
#include "types.h"
#include "parser/vars.h"
#include "parser/exprs.h"

struct type_t* add_array_dimensions(struct type_t *type, int num_dimensions, size_t *dimensions)
{
	struct type_t *t=copy_type(type);
	int n=t->num_arrays;
	t->num_arrays+=num_dimensions;
	t->array_dimensions=realloc(t->array_dimensions, sizeof(size_t)*t->num_arrays);
	memcpy(t->array_dimensions+n, dimensions+n, (t->num_arrays-n)*sizeof(size_t));
	return t;
}
void init_type(struct type_t *t)
{
	t->refcount=1;
	t->native_type=true;
	t->array_dimensions=NULL;
	t->num_arrays=0;
	t->is_constant=false;
	t->is_signed=true;
}

void init_body(struct tbody_t *b)
{
	b->kind=_normal;
	b->refcount=1;
	b->is_func_pointer=false;
	b->core_type=_INT;
}
size_t get_type_size(struct type_t *t);
off_t get_offset_of_member(struct type_t *t, char *name)
{
	struct tbody_t *tb=t->body;
	register int x;
	register off_t o=0;
	register int num_vars=tb->attrs.vars.num_vars;
	size_t alignment=tb->attrs.vars.alignment;
	for (x=0; x<num_vars; x++) {
		if (!strcmp(tb->attrs.vars.vars[x]->name, name))
			return o;
		size_t s=get_type_size(tb->attrs.vars.vars[x]->type);
		if (s<alignment)
			o+=alignment;
		else
			o+=s;
	}
	return -1;
}

bool is_signed(struct type_t *t)
{
	return t->is_signed;
}

struct var_t* get_var_member(struct type_t *t, char *name)
{
	register int x=0;
	struct tbody_t *tb=t->body;
	register int num_vars=tb->attrs.vars.num_vars;
	for (x=0; x<num_vars; x++) {
		if (!strcmp(tb->attrs.vars.vars[x]->name, name))
			return tb->attrs.vars.vars[x];
	}
	return NULL;
}

struct type_t* get_struct_or_union_attr_type(struct type_t *t, char *name)
{
	struct tbody_t *tb=t->body;
	int x;
	for (x=0; x<tb->attrs.vars.num_vars; x++) {
		if (!strcmp(tb->attrs.vars.vars[x]->name, name))
			return tb->attrs.vars.vars[x]->type;
	}

	return NULL;
}

extern void yyerror(char *s);

struct type_t* get_type_by_name(char *name, enum type_kind kind) 
{
	int x;
	for (x=0; x<num_types; x++) {
		struct type_t *t=types[x];
		struct tbody_t *bod=t->body;
		if (!strcmp(t->name, name)) {
			if (bod==NULL)
				return t;

			if (kind==bod->kind)
				return t;
		}
	}
	return NULL;
}


void add_type(struct type_t *t)
{
	num_types++;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=t;
}

size_t get_deref_type_size(struct type_t *t)
{
	int x;
	size_t size;
	if (t->body!=NULL)
		size=t->body->size;
	else
		size=1;

	if (t->array_dimensions!=NULL)
		for (x=0; x<t->num_arrays; x++)
			size*=t->array_dimensions[x];
	return size;
}

size_t get_array_size(struct type_t *t)
{
	if (t->body==NULL)
		return 0;
	int ret=t->body->size;
	int x;
	if (t->num_arrays==0)
		return 0;

	for (x=0; x<t->num_arrays; x++)
		if (t->array_dimensions[x]==0)
			ret=pointer_size;
		else
			ret*=t->array_dimensions[x];
	return ret;
}

size_t get_type_size(struct type_t *t)
{
	if (t==NULL) {
		fprintf(stderr, "Internal Error: Null pointer passed to get_type_size\n");
		exit(1);
	}

	if (t->body==NULL && t->num_arrays >0)
		return pointer_size;

	else if (t->body==NULL && t->num_arrays<=0)
		return 0;

	if (t->body==NULL && t->num_arrays==0)
		return 0;
	else if (t->body==NULL && t->num_arrays>0)
		return pointer_size;

	if (t->num_arrays>0)
		return pointer_size;
	else 
		return t->body->size;
}

int get_type_index_by_name(char *name)
{
	int x;
	for (x=0; x<num_types; x++) {
		if (types[x]!=NULL)
			if (!strcmp(name, types[x]->name))
				return x;
	}
	return -1;
}

void free_type(struct type_t *t);
void free_tbody(struct tbody_t *t)
{
	if (t==NULL)
		return;

	t->refcount--;
	if (t->refcount==0) {
		if (t->kind==_union || t->kind==_struct) {
			int x;
			for (x=0; x<t->attrs.vars.num_vars; x++) {
				free_var(t->attrs.vars.vars[x]);
			}
			free(t->attrs.vars.vars);
		} else if (t->is_func_pointer) {
			free_type(t->attrs.func_ptr.return_type);
			int x;
			for (x=0; x<t->attrs.func_ptr.num_arguments; x++) {
				free_type(t->attrs.func_ptr.arguments[x]);
			}
			free(t->attrs.func_ptr.arguments);
		}
		free(t);
	}
}

void free_native_type(struct type_t *t)
{
	free(t->name);
	free_tbody(t->body);
	free(t);
}

void free_type(struct type_t *t)
{
	if (t==NULL)
		return;

	if (t->native_type)
		return;

	t->refcount--;
	if (t->refcount>0)
		return;


	free_native_type(t);
}

void free_all_types()
{
	int x;
	for (x=0; x<num_types; x++) {
		types[x]->native_type=false;
		free_type(types[x]);
		types[x]=NULL;
	}
	free(types);
	types=NULL;
	num_types=0;
}

void parser_type_cmp(struct expr_t **a, struct expr_t **b)
{
	if (a==NULL || b==NULL) {
		yyerror("Internal error: Expression is NULL when passed to parser_type_cmp)");
		exit(1);
	}

	if ((*a)->type==NULL || (*b)->type==NULL) {
		yyerror("Internal error: Expression is missing type when passed to parser_type_cmp");
		exit(1);
	}

	if (get_type_size((*a)->type) > get_type_size((*b)->type)) {
		*b=convert_expr(*b, (*a)->type);
	} else if (get_type_size((*a)->type) < get_type_size((*b)->type)) {
		*a=convert_expr(*a, (*b)->type);
	}
}

struct type_t* increase_type_depth(struct type_t *t, int n)
{
	struct type_t *new=malloc(sizeof(struct type_t));
	memcpy(new, t, sizeof(struct type_t));
	new->native_type=false;
	new->name=strdup(t->name);
	new->refcount=1;
	int num_elements=t->num_arrays+n;
	new->array_dimensions=calloc(num_elements, sizeof(size_t));
	if (t->array_dimensions!=NULL)
		memcpy(new->array_dimensions, t->array_dimensions, t->num_arrays*sizeof(size_t));
	int x;
	for (x=t->num_arrays; x<num_elements; x++) {
		new->array_dimensions[x]=0;
	}
	new->num_arrays=num_elements;

	if (new->body!=NULL)
		new->body->refcount++;
	return new;
}

bool is_complete_type(struct type_t *t)
{
	return !(t->body==NULL);
}
struct type_t* decrease_type_depth(struct type_t *t, int n)
{
	struct type_t *new=malloc(sizeof(struct type_t));
	memcpy(new, t, sizeof(struct type_t));
	new->refcount=1;
	new->native_type=false;
	int num_elements=t->num_arrays-n;
	new->array_dimensions=calloc(num_elements, sizeof(size_t));
	memcpy(new->array_dimensions, t->array_dimensions, num_elements*sizeof(size_t));
	new->num_arrays=num_elements;
	if (new->body!=NULL)
		new->body->refcount++;
	new->name=strdup(t->name);
	return new;
}

size_t get_alignof(struct type_t *t)
{
	return t->body->attrs.vars.alignment;
}

struct tbody_t* copy_body(struct tbody_t *b)
{
	struct tbody_t *ret=malloc(sizeof(struct tbody_t));
	memcpy(ret, b, sizeof(struct tbody_t));
	ret->refcount=1;
	return ret;
}

struct type_t* copy_type(struct type_t *t)
{
	struct type_t *ret=malloc(sizeof(struct type_t));
	memcpy(ret, t, sizeof(struct type_t));
	ret->name=strdup(t->name);
	if (ret->array_dimensions!=NULL) {
		ret->array_dimensions=calloc(ret->num_arrays, sizeof(size_t));
		memcpy(ret->array_dimensions, t->array_dimensions, ret->num_arrays*sizeof(size_t));
	}
	ret->native_type=false;
	ret->refcount=1;
	ret->body->refcount++;
	return ret;
}
