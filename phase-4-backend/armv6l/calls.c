#define IN_BACKEND
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/globals.h"
#include "globals.h"
#include "types.h"
#include "backend/backend.h"
#include "backend/registers.h"
#include "stack.h"

static struct stack_t *pushed_registers=NULL;

static bool doing_inline=false;
static struct func_t *current_func=NULL;
static int current_arg=0;

void expand_stack_space(FILE *fd, off_t off)
{
	if (off!=0)
		fprintf(fd, "\tsubq $%ld, %%rsp\n", off);
	current_stack_offset+=off;
}

static struct reg_t* get_reg_by_name(char *name)
{
	int x;
	for (x=0; x<num_regs; x++) {
		int y;
		for (y=0; y<regs[x]->num_sizes; y++) {
			if (!strcmp(name, regs[x]->sizes[y].name)) {
				return regs[x];
			}
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

static char* get_next_call_register(enum reg_use r)
{
	struct reg_t *ret;
	char *rname;
	int x=0;
	if (r==INT) {
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
	} else {
		for (x=0; x<8; x++) {
			switch (x) {
			case 0:
				rname="%xmm0";
				break;
			case 1:
				rname="%xmm1";
				break;
			case 2:
				rname="%xmm2";
				break;
			case 3:
				rname="%xmm3";
				break;
			case 4:
				rname="%xmm4";
				break;
			case 5:
				rname="%xmm5";
				break;
			case 6:
				rname="%xmm6";
				break;
			case 7:
				rname="%xmm7";
				break;
			}
			ret=get_reg_by_name(rname);
			if (ret!=NULL && !ret->used_for_call)
				break;
		}
	}

	assert(ret!=NULL);
	ret->used_for_call=true;
	return rname;

}

static void push_registers(FILE *fd)
{

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
}

void start_func_ptr_call(FILE *fd, struct reg_t *r)
{
	reset_used_for_call();
	push_registers(fd);
}

static bool has_float=false;
void start_call(FILE *fd, struct func_t *f)
{
	has_float=false;
	current_func=f;
	if (f->do_inline) {
		doing_inline=true;
		current_arg=0;
		return;
	}
	reset_used_for_call();
	push_registers(fd);
	if (f->num_arguments==0 && f->ret_type->body->kind==_struct) {
		return;
	}
}

void add_argument(FILE *fd, struct reg_t *reg, struct type_t *t )
{
	if (doing_inline) {
		assign_var(fd, reg, current_func->arguments[current_arg]);
		current_arg++;
		return;
	}
	/*TODO: Fix this to work better. */	
	char *next=NULL;
	if (t->body->core_type==_FLOAT) {
		has_float=true;
		next=get_next_call_register(FLOAT);
	} else
		next=get_next_call_register(INT);

	assert(next!=NULL);
	if (t->body->core_type!=_FLOAT) {
		char *name=get_reg_name(reg, pointer_size);

		if (strcmp(name, next))
			fprintf(fd, "\tmovq %s, %s\n", name, next);
	} else {
		char *name=get_reg_name(reg, word_size);
		if (strcmp(name, next))
			fprintf(fd, "\tmovss %s, %s\n", name, next);
	}

	current_arg++;
}

static void pop_registers(FILE *fd)
{
	int x;
	for (x=0; x<num_regs; x++) {
		regs[x]->used_for_call=false;
	}
	if (pushed_registers==NULL)
		return;

	struct reg_t *r2=pop(pushed_registers);
	do {
		int y;
		size_t biggest_size=0;
		for (y=0; y<r2->num_sizes; y++)
			if (r2->sizes[y].size>biggest_size)
				biggest_size=r2->sizes[y].size;
		fprintf(fd, "\tpopq %s\n", get_reg_name(r2, biggest_size));
		if (pushed_registers!=NULL) 
			r2=pop(pushed_registers);
	} while (pushed_registers!=NULL);
}

void call_function_pointer(FILE *fd, struct reg_t *r)
{
	fprintf(fd, "\tcall *%s\n", get_reg_name(r, pointer_size));
	pop_registers(fd);

}

void call(FILE *fd, struct func_t *f)
{

	if (has_float)
		fprintf(fd, "\tmovl $1, %%eax\n");
	else
		fprintf(fd, "\tmovl $0, %%eax\n");
	fprintf(fd, "\tcall %s\n", f->name);

	pop_registers(fd);
	current_arg=0;
}

void return_from_call(FILE *fd)
{
	fprintf(fd, "\tsub\tsp, fp, #0\n");
	fprintf(fd, "\tldr\tfp, [sp], #4\n");
	fprintf(fd, "\tbx\tlr\n");
}

void make_function(FILE *fd, struct func_t *f)
{
	fprintf(fd, "\t.text\n");
	if (f->attributes & _static)
		fprintf(fd, "%s:\n", f->name);
	else
		fprintf(fd, "\t.globl %s\n%s:\n", f->name, f->name);
	fprintf(fd, "\tstr\tfp, [sp, #-4]!\n");
	fprintf(fd, "\tadd\tfp, sp, #0\n");
	off_t o=get_var_offset(f->statement_list, 0);
	o+=8;
	fprintf(fd, "\tsub\tsp, sp, #%zd\n", o);
	if (!strcmp("main", f->name))
		in_main=true;
	else
		multiple_functions=true;

	//fprintf(fd, "\tsubq $16, %%rsp\n");
	/* off_t o=get_var_offset(f->statement_list, 0);
	expand_stack_space(fd, o);
	int x, y=0;
	for (x=0; x<f->num_arguments; x++) {
		size_t size=get_type_size(f->arguments[x]->type);
		if (size==word_size || size==pointer_size) {
			fprintf(fd, "\tsubq $%d, %%rsp\n", pointer_size);
			fprintf(fd, "\tmovq %s, -%d(%%rbp)\n", get_next_call_register(INT), pointer_size);
			current_stack_offset+=pointer_size;
			f->arguments[x]->offset=-pointer_size;
			y++;
		} else {
			f->arguments[x]->offset=8*y+16;
			y++;
		}
	}

	for (x=0; x<num_regs; x++)
		regs[x]->used_for_call=false; */

	in_main=false;
}

void load_function_ptr(FILE *fd, struct func_t *f, struct reg_t *r)
{
	fprintf(fd, "\tmovq $%s, %s\n", f->name, reg_name(r));
}
