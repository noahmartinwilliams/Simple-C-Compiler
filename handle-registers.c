#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator-globals.h"
#include "generator-types.h"
#include "generator.h"
#include "globals.h"
#include "types.h"

struct reg_t **regs=NULL;
int num_regs=0;


/* TODO: setup register stack */

void setup_registers()
{
	num_regs+=4;
	regs=realloc(regs, num_regs*sizeof(struct reg_t*));
	regs[num_regs-1]=malloc(sizeof(struct reg_t));
	regs[num_regs-1]->name=strdup("%eax");
	regs[num_regs-1]->size=word_size;
	regs[num_regs-1]->in_use=true;
	regs[num_regs-1]->use=RET;
	regs[num_regs-1]->pushable_name=strdup("%rax");

	regs[num_regs-2]=malloc(sizeof(struct reg_t));
	regs[num_regs-2]->name=strdup("%ebx");
	regs[num_regs-2]->size=word_size;
	regs[num_regs-2]->in_use=false;
	regs[num_regs-2]->use=INT;
	regs[num_regs-2]->pushable_name=strdup("%rbx");

	regs[num_regs-3]=malloc(sizeof(struct reg_t));
	regs[num_regs-3]->name=strdup("%ecx");
	regs[num_regs-3]->size=word_size;
	regs[num_regs-3]->in_use=false;
	regs[num_regs-3]->use=INT;
	regs[num_regs-3]->pushable_name=strdup("%rcx");

	regs[num_regs-4]=malloc(sizeof(struct reg_t));
	regs[num_regs-4]->name=strdup("%edx");
	regs[num_regs-4]->size=word_size;
	regs[num_regs-4]->in_use=false;
	regs[num_regs-4]->use=INT;
	regs[num_regs-4]->pushable_name=strdup("%rdx");
}

void free_all_registers()
{
	int x;
	for (x=0; x<num_regs; x++) {
		free(regs[x]->name);
		free(regs[x]->pushable_name);
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

struct reg_t* get_free_register(FILE *fd, size_t s)
{
	int x;
	for (x=0; x<num_regs; x++) {
		if (!regs[x]->in_use && regs[x]->size==s) {
			regs[x]->in_use=true;
			return regs[x];
		}
	}

	for (x=0; x<num_regs; x++) {
		if(regs[x]->size==s && regs[x]->depth < depth) {
			fprintf(fd, "\tpushq %s\n", regs[x]->pushable_name);
			regs[x]->depth++;
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
	fprintf(fd, "\tsubl %s, %s\n", a->name, b->name);
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

void free_register(FILE *fd, struct reg_t *r)
{
	if (r->depth==0)
		r->in_use=false;
	else {
		fprintf(fd, "\tpopq %s\n", r->pushable_name);
		r->depth--;
	}
}
