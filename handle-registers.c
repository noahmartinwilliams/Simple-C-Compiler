#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include "generator.h"
#include "generator-types.h"

struct reg_t **regs=NULL;
int num_regs=0;

void setup_registers()
{
	num_regs+=3;
	regs=realloc(regs, num_regs*sizeof(struct reg_t*));
	regs[num_regs-1]=malloc(sizeof(struct reg_t));
	regs[num_regs-1]->name=strdup("%eax");
	regs[num_regs-1]->size=word_size;
	regs[num_regs-1]->in_use=true;
	regs[num_regs-1]->use=RET;

	regs[num_regs-2]=malloc(sizeof(struct reg_t));
	regs[num_regs-2]->name=strdup("%ebx");
	regs[num_regs-2]->size=word_size;
	regs[num_regs-2]->in_use=false;
	regs[num_regs-2]->use=INT;

	regs[num_regs-3]=malloc(sizeof(struct reg_t));
	regs[num_regs-3]->name=strdup("%ecx");
	regs[num_regs-3]->size=word_size;
	regs[num_regs-3]->in_use=false;
	regs[num_regs-3]->use=INT;
}

void free_all_registers()
{
	int x;
	for (x=0; x<num_regs; x++) {
		free(regs[x]->name);
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
		if (regs[x]->use==RET && regs[x]->size==s)
			return regs[x];
	}
	return NULL;
}

struct reg_t* get_free_register(size_t s)
{
	int x;
	for (x=0; x<num_regs; x++) {
		if (!regs[x]->in_use && regs[x]->size==s) {
			regs[x]->in_use=true;
			return regs[x];
		}
	}
	return NULL;
}

void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	fprintf(fd, "\tmovl %s, %s\n", src->name, dest->name);
}

void add(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\taddl %s, %s\n", a->name, b->name);
}

void sub(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\tsubq %s, %s\n", a->name, b->name);
}

void expand_stack_space(FILE *fd, off_t off)
{
	fprintf(fd, "\tsubq $%ld, %%rsp\n", off);
}

void read_var(FILE *fd, struct var_t *v)
{
	fprintf(fd, "\tmovl %ld(%%rbp), %%eax\n", v->offset);
}

void assign_constant(FILE *fd, struct expr_t *e)
{
	fprintf(fd, "\tmovl $%ld, %%eax\n", e->attrs.cint_val);
}
