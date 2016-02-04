#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/globals.h"
#include "generator/types.h"
#include "generator/generator.h"
#include "globals.h"
#include "types.h"
#include "backend/backend.h"
#include "backend/registers.h"
#include "handle-types.h"

extern bool last_comparison_float;
static inline void size_error(char *message, size_t size)
{
	fprintf(stderr, "Internal Error: unknown size: %ld passed to %s\n", size, message);
	exit(1);
}

void compare_float_registers(FILE *fd, struct reg_t *b, struct reg_t *a)
{
	last_comparison_float=true;
	fprintf(fd, "\tucomisd %s, %s\n", reg_name(b), reg_name(a));
}

struct depth_value {
	int depth;
	char *name;
};

struct reg_t* get_free_float_register(FILE *fd, size_t s, int depth)
{
	int x;
	struct reg_t *r;
	for (x=0; x<num_regs; x++) {
		r=regs[x];
		if (r->in_use==false && r->use==FLOAT)
			return r;
	}
	for (x=0; x<num_regs; x++) {
		struct depth_value *current_depth=malloc(sizeof(struct depth_value));
		r=regs[x];
		current_depth->depth=depth;
		current_depth->name=reg_name(r);
		struct depth_value *reg_depth=pop((r->depths));
		if (reg_depth->depth < depth && r->use==FLOAT) {
			push((r->depths), reg_depth);
			fprintf(fd, "\tpushsd %s\n", r->sizes[0].name);
			current_stack_offset+=16;
			push((r->depths), current_depth);
			return regs[x];
		}
	}
	return NULL;

}

void free_float_register(FILE *fd, struct reg_t *reg)
{
	if (reg->depths!=NULL) {
		struct depth_value *dep_val=pop((reg->depths));
		fprintf(fd, "\tpopsd %s\n", dep_val->name);
		free(dep_val);
	}

	if (reg->depths==NULL)
		reg->in_use=false;
}

struct reg_t* get_float_ret_register(size_t s)
{
	int x;
	for (x=0; x<num_regs; x++) {
		struct reg_t *r=regs[x];
		if (r->use==FLOAT_RET)
			return r;
	}
	return NULL;
}

void assign_constant_float(FILE *fd, struct expr_t *e)
{
	fprintf(fd, "\tmovsd %s(%%rip), %%xmm0\n", e->attrs.cfloat);
}

void read_float_var(FILE *fd, struct var_t *v)
{
	if (v->scope_depth==0)
		fprintf(fd, "\tcvtss2sd %s(%%rip), %%xmm0\n", v->name);
	else
		fprintf(fd, "\tcvtss2sd %d(%%rbp), %%xmm0\n", v->offset);
}

static void arithmetic_intern(FILE *fd, char *name, struct reg_t *src, struct reg_t *dest)
{
	fprintf(fd, "\t%s %s, %s\n", name, reg_name(src), reg_name(dest));
	if (dest->use!=FLOAT_RET)
		fprintf(fd, "\tmovsd %s, %s\n", reg_name(dest), reg_name(get_float_ret_register(word_size)));
}

void float_add(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	arithmetic_intern(fd, "addsd", src, dest);
}

void float_sub(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	arithmetic_intern(fd, "subsd", src, dest);
}

void float_mul(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	arithmetic_intern(fd, "mulsd", src, dest);
}

void float_div(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	fprintf(fd, "\tdivsd %s, %s\n", reg_name(dest), reg_name(src));
	fprintf(fd, "\tmovsd %s, %%xmm0\n", reg_name(src));

}

char* generate_global_float(FILE *fd, char *num)
{
	fprintf(fd, ".section .rodata\n");
	unique_num++;
	fprintf(fd, "\t.LC%d:\n", unique_num);
	float f=atof(num);
	fprintf(fd, "\t.double %e\n", f);
	char *n=NULL;
	asprintf(&n, ".LC%d", unique_num);
	return n;
}
