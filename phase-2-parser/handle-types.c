#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "generator/globals.h"
#include "types.h"
#include "handle-vars.h"

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
struct type_t* get_type_by_name(char *name)
{
	int x;
	for (x=0; x<num_types; x++) {
		if (!strcmp(name, types[x]->name) && types[x]->body==NULL)
			return types[x];

		if (!strcmp(name, types[x]->name) && !types[x]->body->is_struct)
			return types[x];
	}
	return NULL;
}

struct type_t* get_struct_by_name(char *name)
{
	register int x;
	for (x=0; x<num_types; x++) {
		if (!strcmp(name, types[x]->name) && types[x]->body->is_struct)
			return types[x];
	}
	return NULL;
}

void add_type(struct type_t *t)
{
	num_types++;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=t;
}

size_t get_type_size(struct type_t *t)
{
	if (t==NULL) {
		fprintf(stderr, "Internal Error: Null pointer passed to get_type_size\n");
		exit(1);
	}

	if (t->body==NULL && t->pointer_depth >0)
		return pointer_size;

	else if (t->body==NULL && t->pointer_depth<=0)
		return 0;

	if (t->body==NULL && t->pointer_depth==0)
		return 0;
	else if (t->body==NULL && t->pointer_depth>0)
		return pointer_size;

	if (t->pointer_depth>0)
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
		if (t->is_struct || t->is_union) {
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
	if ((*a)->type->pointer_depth!=(*b)->type->pointer_depth) {
		yyerror("Type mismatch");
		exit(1);
	}
}

struct type_t* increase_type_depth(struct type_t *t, int n)
{
	struct type_t *new=malloc(sizeof(struct type_t));
	memcpy(new, t, sizeof(struct type_t));
	new->native_type=false;
	new->name=strdup(t->name);
	new->refcount=1;
	new->pointer_depth+=n;
	if (new->body!=NULL)
		new->body->refcount++;
	return new;
}

struct type_t* decrease_type_depth(struct type_t *t, int n)
{
	struct type_t *new=malloc(sizeof(struct type_t));
	memcpy(new, t, sizeof(struct type_t));
	new->refcount=1;
	new->native_type=false;
	new->pointer_depth-=n;
	if (new->body!=NULL)
		new->body->refcount++;
	new->name=strdup(t->name);
	return new;
}

size_t get_alignof(struct type_t *t)
{
	return t->body->attrs.vars.alignment;
}
