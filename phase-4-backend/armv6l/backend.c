#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser/types.h"
#include "generator/types.h"
#include "generator/generator.h"
#include "generator/globals.h"
#include "globals.h"
#include "types.h"
#include "backend/globals.h"

void size_error(char *message, size_t size)
{
	fprintf(stderr, "Internal Error: unknown size: %ld passed to %s\n", size, message);
	exit(1);
}

static void init_reg(struct reg_t *reg)
{
	reg->is_available=true;
	reg->depths=NULL;
	reg->in_use=false;
	reg->is_signed=true;
}

void prepare_for_new_function(FILE *fd)
{
	int x=0;
	for (x=0; x<num_regs; x++)
		init_reg(regs[x]);
}

void setup_backend()
{
	num_regs=13;
	regs=calloc(num_regs, sizeof(struct reg_t*));
	int x;
	for (x=0; x<13; x++) {
		regs[x]=malloc(sizeof(struct reg_t));
		regs[x]->size=4; //purely guessing here.
		regs[x]->num_sizes=1;
		regs[x]->sizes=malloc(sizeof(struct reg_size));
		regs[x]->sizes->size=4;
		asprintf(&(regs[x]->sizes->name), "r%d", x);
		init_reg(regs[x]);
		regs[x]->use=INT;
	}
	regs[0]->use=RET;
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
	fprintf(fd, "\t@%s\n", str);
}

void add_readonly_data(FILE *fd, struct expr_t *e)
{
	unique_num++;
	fprintf(fd, "\t.section\t.rodata\n");
	fprintf(fd, "\t.LC%d:\n", unique_num);
	/* TODO: figure this part out */
}
