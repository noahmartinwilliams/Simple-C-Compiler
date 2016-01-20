#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/generator-globals.h"
#include "generator/generator-types.h"
#include "globals.h"
#include "types.h"

struct reg_t **regs=NULL;
int num_regs=0;

char* get_reg_name(struct reg_t *reg, size_t size)
{
	int x;
	for (x=0; x<reg->num_sizes; x++) {
		if (reg->sizes[x].size==size)
			return reg->sizes[x].name;
	}
}

char* reg_name(struct reg_t *a)
{
	return get_reg_name(a, a->size);
}

struct reg_t* get_ret_register(size_t s)
{
	if (regs==NULL) {
		fprintf(stderr, "internal error: registers empty\n");
		exit(1);
	}
	int x;
	for (x=0; x<num_regs; x++) {
		if (regs[x]->use==RET) {
			regs[x]->size=s;
			return regs[x];
		}
	}
	return NULL;
}

struct depth_value {
	int depth;
	char *name;
};

struct reg_t* get_free_register (FILE *fd, size_t s, int depth)
{
	int x;
	for (x=0; x<num_regs; x++) {
		if (!regs[x]->in_use && regs[x]->use==INT) {
			struct depth_value *current_depth=malloc(sizeof(struct depth_value));
			current_depth->depth=depth;
			current_depth->name=reg_name(regs[x]);
			regs[x]->in_use=true;
			regs[x]->size=s;
			push((regs[x]->depths), current_depth);
			return regs[x];
		}
	}

	for (x=0; x<num_regs; x++) {
		struct depth_value *current_depth=malloc(sizeof(struct depth_value));
		current_depth->depth=depth;
		current_depth->name=reg_name(regs[x]);
		if(regs[x]->size==s && current_depth->depth < depth && regs[x]->use==INT) {
			fprintf(fd, "\tpushq %s\n", regs[x]->sizes[3].name);
			push((regs[x]->depths), current_depth);
			int *new_depth=malloc(sizeof(int));
			*new_depth=depth;
			push((regs[x]->depths), new_depth);
			return regs[x];
		}
	}

	return NULL;
}

void free_register(FILE *fd, struct reg_t *r)
{
	if (r->depths!=NULL && r->depths->next==NULL) {
		free(pop(r->depths));
		r->in_use=false;
	} else {
		struct depth_value *dv=pop(r->depths);
		fprintf(fd, "\tpopq %s\n", dv->name);
		free(dv);
	}
}

void set_register_size(struct reg_t *r, size_t s)
{
	r->size=s;
}
