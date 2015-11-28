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


char* get_reg_name(struct reg_t *reg, size_t size);
void setup_registers()
{
	num_regs+=4;
	regs=realloc(regs, num_regs*sizeof(struct reg_t*));

	char *primary[]={"a", "b", "c", "d"};
	int x;
	for (x=0; x<4; x++) {
		regs[x]=malloc(sizeof(struct reg_t));
		regs[x]->sizes=calloc(4, sizeof(struct reg_size));
		regs[x]->num_sizes=4;
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
}

void dereference(FILE *fd, struct reg_t *reg, size_t size)
{
	if (size==word_size)
		fprintf(fd, "\tmovl (%s), %%eax\n", get_reg_name(reg, reg->size));
	else if (size==pointer_size)
		fprintf(fd, "\tmovq (%s), %%rax\n", get_reg_name(reg, reg->size));
}

char* get_reg_name(struct reg_t *reg, size_t size)
{
	int x;
	for (x=0; x<reg->num_sizes; x++) {
		if (reg->sizes[x].size==size)
			return reg->sizes[x].name;
	}
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

void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (src->size==word_size)
		fprintf(fd, "\tmovl %s, %s\n", get_reg_name(src, src->size), get_reg_name(dest, dest->size));
	else if (src->size==pointer_size)
		fprintf(fd, "\tmovq %s, %s\n", get_reg_name(src, src->size), get_reg_name(dest, dest->size));
}

void assign_var(FILE *fd, struct reg_t *src, struct var_t *dest)
{
	fprintf(fd, "\tmovl %s, %ld(%%rbp)\n", get_reg_name(src, src->size), dest->offset);
} /* TODO: put this to use*/

void int_add(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		fprintf(fd, "\taddl %s, %s\n", get_reg_name(a, word_size), get_reg_name(b, word_size));
		fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, word_size));
	} else if (a->size==pointer_size) {
		fprintf(fd, "\taddq %s, %s\n", get_reg_name(a, pointer_size), get_reg_name(b, pointer_size));
		fprintf(fd, "\tmovq %s, %%rax\n", get_reg_name(b, pointer_size));
	}
}

void int_inc_by(FILE *fd, struct reg_t *a, char *dest)
{
	if (a->size==word_size)
		fprintf(fd, "\taddl %s, %s\n", get_reg_name(a, a->size), dest);
	else if (a->size==pointer_size)
		fprintf(fd, "\taddq %s, %s\n", get_reg_name(a, a->size), dest);

}

void inc_by_int(FILE *fd, int i, char *dest, size_t size)
{
	if (size==word_size)
		fprintf(fd, "\taddl $%d, %s\n", i, dest);
	else if (size==pointer_size)
		fprintf(fd, "\taddq $%d, %s\n", i, dest);
}

void int_sub(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\tsubl %s, %s\n", get_reg_name(b, b->size), get_reg_name(a, a->size));
	fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, b->size));
}

void int_div(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	/* The X86 handles integer division in some stupid way.
	For some reason the div instruction takes just one argument, and
	DX:AX is the numerator, and the argument is the denominator.
	This code has to be complicated to deal with that nonsense. */
	fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(a, a->size));
	fprintf(fd, "\tcltd\n");
	fprintf(fd, "\tidivl %s\n", get_reg_name(b, b->size));

}

void int_mul(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (strcmp(a->sizes[0].name, "%al"))
		fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(a, a->size));
	fprintf(fd, "\tmull %s\n", get_reg_name(b, b->size));
}

void expand_stack_space(FILE *fd, off_t off)
{
	fprintf(fd, "\tsubq $%ld, %%rsp\n", off);
}

void read_var(FILE *fd, struct var_t *v)
{
	if (v->type->pointer_depth > 0)
		fprintf(fd, "\tmovq %ld(%%rbp), %%rax\n", v->offset);
	else {
		if (v->type->body->size==word_size)
			fprintf(fd, "\tmovl %ld(%%rbp), %%eax\n", v->offset);
		if (v->type->body->size==pointer_size)
			fprintf(fd, "\tmovq %ld(%%rbp), %%rax\n", v->offset);
	}
}

void assign_constant(FILE *fd, struct expr_t *e)
{
	if (e->type->pointer_depth>0)
		fprintf(fd, "\tmovq $%ld, %%eax\n", e->attrs.cint_val);
	if (e->type->body->size==word_size && e->type->pointer_depth==0)
		fprintf(fd, "\tmovl $%ld, %%eax\n", e->attrs.cint_val);
}

void assign_constant_int(FILE *fd, int e)
{
	fprintf(fd, "\tmovl $%d, %%eax\n", e);
}

void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\tcmpl %s, %s\n", get_reg_name(a, a->size), get_reg_name(b, b->size));
}

void compare_register_to_int(FILE *fd, struct reg_t *a, int i)
{
	fprintf(fd, "\tcmpl $%d, %s\n", i, get_reg_name(a, a->size));
}

void jmp(FILE *fd, char *name)
{
	fprintf(fd, "\tjmp %s\n", name);
}

void jmp_eq(FILE *fd, char *name)
{
	fprintf(fd, "\tje %s\n", name);
}

void jmp_neq(FILE *fd, char *name)
{
	fprintf(fd, "\tjne %s\n", name);
}

void jmp_lt(FILE *fd, char *name)
{
	fprintf(fd, "\tjl %s\n", name);
}

void jmp_gt(FILE *fd, char *name)
{
	fprintf(fd, "\tjg %s\n", name);
}

void place_label(FILE *fd, char *name)
{
	fprintf(fd, "\t%s:\n", name);
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
