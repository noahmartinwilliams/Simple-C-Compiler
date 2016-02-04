#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/generator-globals.h"
#include "generator/generator-types.h"
#include "globals.h"
#include "types.h"
#include "backend/float-arithmetic.h"

struct reg_t **regs=NULL;
int num_regs=0;

char* get_reg_name(struct reg_t *reg, size_t size)
{
	if (reg->use==FLOAT || reg->use==FLOAT_RET) 
		return reg->sizes[0].name;

	int x;
	for (x=0; x<reg->num_sizes; x++)
		if (reg->sizes[x].size==size)
			return reg->sizes[x].name;
}

char* reg_name(struct reg_t *a)
{
	return get_reg_name(a, a->size);
}

struct reg_t* get_ret_register(size_t s, bool is_float)
{
	if (is_float)
		return get_float_ret_register(s);

	if (regs==NULL) {
		fprintf(stderr, "internal error: registers empty\n");
		exit(1);
	}
	int x;
	for (x=0; x<num_regs; x++)
		if (regs[x]->use==RET) {
			regs[x]->size=s;
			return regs[x];
		}

	return NULL;
}

struct depth_value {
	int depth;
	char *name;
};

struct reg_t* get_free_register (FILE *fd, size_t s, int depth, bool is_float)
{
	if (is_float)
		return get_free_float_register(fd, s, depth);

	int x;
	struct reg_t *r;
	for (x=0; x<num_regs; x++) {
		r=regs[x];
		if (!r->in_use && r->use==INT) {
			regs[x]->in_use=true;
			regs[x]->size=s;
			return r;
		}
	}

	for (x=0; x<num_regs; x++) {
		r=regs[x];
		struct depth_value *current_depth=malloc(sizeof(struct depth_value));
		current_depth->depth=depth;
		current_depth->name=get_reg_name(r, pointer_size);
		struct depth_value *reg_depth=pop((r->depths));
		if (r->size==s && reg_depth->depth < depth && r->use==INT) {

			push((r->depths), reg_depth);
			fprintf(fd, "\tpushq %s\n", get_reg_name(r, pointer_size));
			current_stack_offset+=pointer_size; /*TODO: fix this for future architecutres */
			push((r->depths), current_depth);

			return r;
		}
	}

	return NULL;
}

void free_register(FILE *fd, struct reg_t *r)
{
	if (r->use==FLOAT)
		free_float_register(fd, r);

	struct depth_value *dv;
	if (r->depths!=NULL && r->depths->next==NULL) {
		dv=pop(r->depths);
		fprintf(fd, "\tpopq %s\n", dv->name);
		current_stack_offset-=pointer_size;
		free(dv);
		r->in_use=false;
	} else if (r->depths!=NULL && r->depths->next!=NULL) {
		dv=pop(r->depths);
		fprintf(fd, "\tpopq %s\n", dv->name);
		current_stack_offset-=pointer_size;
		free(dv);
	} else
		r->in_use=false;
}

void set_register_size(struct reg_t *r, size_t s)
{
	if (r->use==FLOAT)
		return;

	r->size=s;
}
