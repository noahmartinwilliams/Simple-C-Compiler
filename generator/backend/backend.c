#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/generator-globals.h"
#include "generator/generator-types.h"
#include "generator/generator.h"
#include "globals.h"
#include "types.h"
#include "generator/backend/registers.h"

void setup_backend()
{
	num_regs+=14;
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
		for (y=0; y<regs[x]->num_sizes; y++) {
			free_reg_size(regs[x]->sizes[y]);
		}
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
	else {
		fprintf(stderr, "Internal Error: unknown size: %ld passed to assign_constant\n", get_type_size(e->type));
		exit(1);
	}
}

void assign_constant_int(FILE *fd, int e)
{
	fprintf(fd, "\tmovl $%d, %%eax\n", e);
}

void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==char_size)
		fprintf(fd, "\tcmpb %s, %s\n", reg_name(a), reg_name(b));

	else if (a->size==word_size)
		fprintf(fd, "\tcmpl %s, %s\n", reg_name(a), reg_name(b));

	else if (a->size==pointer_size)
		fprintf(fd, "\tcmpq %s, %s\n", reg_name(a), reg_name(b));
}

void compare_register_to_int(FILE *fd, struct reg_t *a, int i)
{
	if (a->size==char_size)
		fprintf(fd, "\tcmpb $%d, %s\n", i, reg_name(a));

	else if (a->size==word_size)
		fprintf(fd, "\tcmpl $%d, %s\n", i, reg_name(a));
	
	else if (a->size==pointer_size)
		fprintf(fd, "\tcmpq $%d, %s\n", i, reg_name(a));
}

void add_readonly_data(FILE *fd, struct expr_t *e)
{
	unique_num++;
	fprintf(fd, "\t.section\t.rodata\n");
	fprintf(fd, "\t.LC%d:\n", unique_num);
	/* TODO: figure this part out */
}
