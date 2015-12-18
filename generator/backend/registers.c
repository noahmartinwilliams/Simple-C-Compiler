#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator-globals.h"
#include "generator-types.h"
#include "generator.h"
#include "globals.h"
#include "types.h"

char* get_reg_name(struct reg_t *reg, size_t size);
void setup_registers()
{
	num_regs+=12;
	regs=realloc(regs, num_regs*sizeof(struct reg_t*));

	char *primary[]={"a", "b", "c", "d"};
	int x;
	for (x=0; x<4; x++) {
		regs[x]=malloc(sizeof(struct reg_t));
		regs[x]->sizes=calloc(4, sizeof(struct reg_size));
		regs[x]->num_sizes=4;
		regs[x]->size=8;
		regs[x]->in_use=false;
		regs[x]->depth=0;
		asprintf(&(regs[x]->sizes[0].name), "%%%sl", primary[x]);
		asprintf(&(regs[x]->sizes[1].name), "%%%sx", primary[x]);
		asprintf(&(regs[x]->sizes[2].name), "%%e%sx", primary[x]);
		asprintf(&(regs[x]->sizes[3].name), "%%r%sx", primary[x]);

		regs[x]->sizes[0].size=1;
		regs[x]->sizes[1].size=2;
		regs[x]->sizes[2].size=4;
		regs[x]->sizes[3].size=8;

		regs[x]->use=INT;
	}
	regs[0]->use=RET;
	for (x=4; x<12; x++) {
		regs[x]=malloc(sizeof(struct reg_t));
		regs[x]->sizes=calloc(4, sizeof(struct reg_size));
		regs[x]->num_sizes=4;
		asprintf(&(regs[x]->sizes[3].name), "%%r%d", (x-4)+8);
		regs[x]->sizes[3].size=8;
		asprintf(&(regs[x]->sizes[0].name), "%%r%dd", (x-4)+8);
		regs[x]->sizes[0].size=4;
		asprintf(&(regs[x]->sizes[1].name), "%%r%dw", (x-4)+8);
		regs[x]->sizes[1].size=2;
		asprintf(&(regs[x]->sizes[2].name), "%%r%db", (x-4)+8);
		regs[x]->sizes[2].size=1;
		regs[x]->in_use=false;
		regs[x]->depth=0;
		regs[x]->use=INT;
	}
}

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
void free_reg_size(struct reg_size a) 
{
	free(a.name);
}

void free_all_registers()
{
	int x;
	for (x=0; x<num_regs; x++) {
		int y;
		for (y=0; y<regs[x]->num_sizes; y++) {
			free_reg_size(regs[x]->sizes[y]);
		}
		free(regs[x]->sizes);
		free(regs[x]);
	}
	free(regs);
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

struct reg_t* get_free_register(FILE *fd, size_t s)
{
	int x;
	for (x=0; x<num_regs; x++) {
		if (!regs[x]->in_use && regs[x]->use==INT) {
			regs[x]->in_use=true;
			regs[x]->size=s;
			return regs[x];
		}
	}

	for (x=0; x<num_regs; x++) {
		if(regs[x]->size==s && regs[x]->depth < depth && regs[x]->use==INT) {
			fprintf(fd, "\tpushq %s\n", regs[x]->sizes[3].name);
			regs[x]->depth++;
			return regs[x];
		}
	}

	return NULL;
}

void free_register(FILE *fd, struct reg_t *r)
{
	if (r->depth==0)
		r->in_use=false;
	else {
		fprintf(fd, "\tpopq %s\n", r->sizes[3].name);
		r->depth--;
	}
}
