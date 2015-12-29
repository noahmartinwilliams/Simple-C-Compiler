#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator-globals.h"
#include "generator-types.h"
#include "generator.h"
#include "globals.h"
#include "types.h"
#include "registers.h"
#include "stack.h"

static struct stack_t *pushed_registers=NULL;

static struct reg_t* get_reg_by_name(char *name)
{
	int x;
	for (x=0; x<num_regs; x++) {
		int y;
		for (y=0; y<regs[x]->num_sizes; y++) {
			if (!strcmp(name, regs[x]->sizes[y].name))
				return regs[x];
		}
	}
	return NULL;
}

static void reset_used_for_call()
{
	int x;
	for (x=0; x<num_regs; x++) {
		regs[x]->used_for_call=false;
	}
}

char* get_next_call_register(enum reg_use r)
{
	struct reg_t *ret;
	char *rname;
	int x=0;
	for (x=0; x<6; x++) {
		switch (x) {
		case 0:
			rname="%rdi";
			break;
		case 1:
			rname="%rsi";
			break;
		case 2:
			rname="%rdx";
			break;
		case 3:
			rname="%rcx";
			break;
		case 4:
			rname="%r8";
			break;
		case 5:
			rname="%r9";
			break;
		}
		ret=get_reg_by_name(rname);
		if (ret!=NULL && !ret->used_for_call)
			break;
	}

	ret->used_for_call=true;
	return rname;

}
void start_call(FILE *fd, struct func_t *f)
{
	reset_used_for_call();
	int x;
	for (x=0; x<num_regs; x++) {
		int y;
		size_t biggest_size=0;
		for (y=0; y<regs[x]->num_sizes; y++)
			if (regs[x]->sizes[y].size>biggest_size)
				biggest_size=regs[x]->sizes[y].size;
		if (regs[x]->in_use) {
			fprintf(fd, "\tpushq %s\n", get_reg_name(regs[x], biggest_size));
			push(pushed_registers, regs[x]);
		}
	}
	if (f->num_arguments==0 && f->ret_type->body->is_struct==false) {
		return;
	}
}

void add_argument(FILE *fd, struct reg_t *reg, struct type_t *t )
{
	/*TODO: Fix this to work better. */	
	char *next=get_next_call_register(INT);
	char *name=get_reg_name(reg, pointer_size);

	if (strcmp(name, next))
		fprintf(fd, "\tmovq %s, %s\n", name, next);
}

void call(FILE *fd, struct func_t *f)
{
	fprintf(fd, "\tcall %s\n", f->name);

	int x;
	for (x=0; x<num_regs; x++) {
		regs[x]->used_for_call=false;
	}
	if (pushed_registers==NULL)
		return;

	struct reg_t *r=pop(pushed_registers);
	for (; pushed_registers!=NULL; r=pop(pushed_registers)) {
		int y;
		size_t biggest_size=0;
		for (y=0; y<r->num_sizes; y++)
			if (r->sizes[y].size>biggest_size)
				biggest_size=r->sizes[y].size;
		fprintf(fd, "\tpopq %s\n", get_reg_name(r, biggest_size));
	}
}

void return_from_call(FILE *fd)
{
	if (in_main && multiple_functions) {
		fprintf(fd, "\tleave\n\tret\n");
	} else {
		fprintf(fd, "\tmovq %%rbp, %%rsp\n");
		fprintf(fd, "\tpopq %%rbp\n");
		fprintf(fd, "\tret\n");
	}
}

void make_function(FILE *fd, struct func_t *f)
{
	fprintf(fd, "\t.text\n");
	if (f->attributes & _static){
		fprintf(fd, "\t.type %s, @function\n%s:\n", f->name, f->name);
	} else {
		fprintf(fd, "\t.globl %s\n\t.type %s, @function\n%s:\n", f->name, f->name, f->name);
	}
	fprintf(fd, "\tpushq %%rbp\n");
	fprintf(fd, "\tmovq %%rsp, %%rbp\n");
	if (!strcmp("main", f->name)) {
		in_main=true;
	} else
		multiple_functions=true;

	off_t o=get_var_offset(f->statement_list, 0);
	int x, y=0;
	/* TODO: adjust this to work with c calling convention for x86_64 */
	for (x=0; x<f->num_arguments; x++) {
		size_t size=get_type_size(f->arguments[x]->type);
		if (size==word_size || size==pointer_size) {
			o=o+pointer_size;
			fprintf(fd, "\tsubq $%d, %%rsp\n", pointer_size);
			fprintf(fd, "\tmovq %s, -%d(%%rbp)\n", get_next_call_register(INT), pointer_size);
			f->arguments[x]->offset=-pointer_size;
			y++;
		} else {
			f->arguments[x]->offset=8*y+16;
			y++;
		}
	}

	for (x=0; x<num_regs; x++) {
		regs[x]->used_for_call=false;
	}

	if (o==0 || multiple_functions) {
		current_stack_offset=0;
		fprintf(fd, "\tsubq $16, %%rsp\n");
		fprintf(fd, "\tmovl $0, -4(%%rbp)\n");
	} else {
		current_stack_offset=o;
		expand_stack_space(fd, o);
	}

	generate_statement(fd, f->statement_list);
	if (!strcmp(f->ret_type->name, "void"))
		return_from_call(fd);

	while (loop_stack!=NULL) {
		struct stack_t *tmp=loop_stack->next;
		free(loop_stack->element);
		free(loop_stack);
		loop_stack=tmp;
	}
	in_main=false;
}

