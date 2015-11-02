#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include "generator.h"

struct reg_t {
	char *name;
	size_t size;
	bool in_use;
} **regs=NULL;
int num_regs=0;

void setup_registers()
{
	num_regs++;
	regs=realloc(regs, num_regs*sizeof(struct reg_t*));
	regs[num_regs-1]=malloc(sizeof(struct reg_t));
	regs[num_regs-1]->name=strdup("%eax");
	regs[num_regs-1]->size=word_size;
	regs[num_regs-1]->in_use=false;
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
