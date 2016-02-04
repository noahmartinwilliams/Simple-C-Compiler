#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handle-types.h"
#include "generator/generator-types.h"
#include "generator/generator.h"
#include "generator/generator-globals.h"
#include "backend/agnostic-arithmetic.h"
#include "backend/int-arithmetic.h"
#include "backend/variables.h"
#include "backend/registers.h"
#include "backend/calls.h"
#include "backend/jumps.h"
#include "globals.h"
#include "types.h"
#include "backend/globals.h"

static inline void size_error(char *message, size_t size)
{
	fprintf(stderr, "Internal Error: unknown size: %ld passed to %s\n", size, message);
	exit(1);
}

void setup_backend()
{

	num_regs=22;
	regs=realloc(regs, num_regs*sizeof(struct reg_t*));

	char *primary[]={"a", "b", "c", "d"};
	int x;
	for (x=0; x<4; x++) {
		regs[x]=malloc(sizeof(struct reg_t));
		regs[x]->sizes=calloc(4, sizeof(struct reg_size));
		regs[x]->num_sizes=4;
		regs[x]->size=8;
		regs[x]->in_use=false;
		regs[x]->depths=NULL;
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
		regs[x]->depths=NULL;
		regs[x]->use=INT;
	}
	regs[x]=malloc(sizeof(struct reg_t));
	regs[x]->sizes=calloc(4, sizeof(struct reg_size));
	regs[x]->num_sizes=4;
	regs[x]->sizes[0].name=strdup("%dil"); // I have no idea if this is right. :/
	regs[x]->sizes[0].size=1;
	regs[x]->sizes[1].name=strdup("%di");
	regs[x]->sizes[1].size=2;
	regs[x]->sizes[2].name=strdup("%edi");
	regs[x]->sizes[2].size=4;
	regs[x]->sizes[3].name=strdup("%rdi");
	regs[x]->sizes[3].size=8;
	regs[x]->in_use=false;
	regs[x]->depths=NULL;
	regs[x]->use=INT;

	x++;
	regs[x]=malloc(sizeof(struct reg_t));
	regs[x]->sizes=calloc(4, sizeof(struct reg_size));
	regs[x]->num_sizes=4;
	regs[x]->sizes[0].name=strdup("%sil"); // I have no idea if this is right. :/
	regs[x]->sizes[0].size=1;
	regs[x]->sizes[1].name=strdup("%si");
	regs[x]->sizes[1].size=2;
	regs[x]->sizes[2].name=strdup("%esi");
	regs[x]->sizes[2].size=4;
	regs[x]->sizes[3].name=strdup("%rsi");
	regs[x]->sizes[3].size=8;
	regs[x]->in_use=false;
	regs[x]->depths=NULL;
	regs[x]->use=INT;

	x++;
	regs[x]=malloc(sizeof(struct reg_t));
	regs[x]->sizes=calloc(1, sizeof(struct reg_size));
	regs[x]->num_sizes=1;
	char *tmp=NULL;
	asprintf(&tmp, "%%xmm%d", x-14);
	regs[x]->sizes[0].name=tmp; 
	regs[x]->sizes[0].size=16;
	regs[x]->in_use=false;
	regs[x]->depths=NULL;
	regs[x]->used_for_call=false;
	regs[x]->use=FLOAT_RET;
	regs[x]->size=16;

	for (x++; x<22; x++) {
		regs[x]=malloc(sizeof(struct reg_t));
		regs[x]->sizes=calloc(1, sizeof(struct reg_size));
		regs[x]->num_sizes=1;
		char *tmp=NULL;
		asprintf(&tmp, "%%xmm%d", x-14);
		regs[x]->sizes[0].name=tmp; 
		regs[x]->sizes[0].size=16;
		regs[x]->in_use=false;
		regs[x]->depths=NULL;
		regs[x]->used_for_call=false;
		regs[x]->use=FLOAT;
		regs[x]->size=16;

	}
}

static void free_reg_size(struct reg_size a) 
{
	free(a.name);
}

void cleanup_backend()
{
	int x;
	for (x=0; x<num_regs; x++) {
		int y;
		for (y=0; y<regs[x]->num_sizes; y++)
			free_reg_size(regs[x]->sizes[y]);

		free(regs[x]->sizes);
		for (; regs[x]->depths!=NULL; free(pop(regs[x]->depths))) {}
		free(regs[x]);
	}
	free(regs);
}

void place_comment(FILE *fd, char *str)
{
	fprintf(fd, "\t#%s\n", str);
}

void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (dest->use==FLOAT || dest->use==FLOAT_RET) {
		fprintf(fd, "\tmovsd %s, %s\n", reg_name(src), reg_name(dest));
		return;
	}
	if (dest->size==char_size)
		fprintf(fd, "\tmovb %s, %s\n", get_reg_name(src, dest->size), reg_name(dest));

	else if (dest->size==word_size)
		fprintf(fd, "\tmovl %s, %s\n", get_reg_name(src, dest->size), reg_name(dest));

	else if (dest->size==pointer_size)
		fprintf(fd, "\tmovq %s, %s\n", get_reg_name(src, dest->size), reg_name(dest));
}

void assign_constant(FILE *fd, struct expr_t *e)
{
	if (get_type_size(e->type)==char_size)
		fprintf(fd, "\tmovb $%ld, %%al\n", e->attrs.cint_val);
	else if (get_type_size(e->type)==word_size)
		fprintf(fd, "\tmovl $%ld, %%eax\n", e->attrs.cint_val);
	else if (get_type_size(e->type)==pointer_size)
		fprintf(fd, "\tmovq $%ld, %%rax\n", e->attrs.cint_val);
	else 
		size_error("assign_constant", get_type_size(e->type));
}

void assign_constant_int(FILE *fd, int e)
{
	fprintf(fd, "\tmovl $%d, %%eax\n", e);
}


void add_readonly_data(FILE *fd, struct expr_t *e)
{
	unique_num++;
	fprintf(fd, "\t.section\t.rodata\n");
	fprintf(fd, "\t.LC%d:\n", unique_num);
	/* TODO: figure this part out */
}
