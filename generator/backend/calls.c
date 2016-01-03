#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/generator-globals.h"
#include "generator/generator-types.h"
#include "generator/generator.h"
#include "globals.h"
#include "types.h"
#include "generator/backend/registers.h"
#include "stack.h"

static struct stack_t *pushed_registers=NULL;

static bool doing_inline=false;
static struct func_t *current_func=NULL;
static int current_arg=0;

static void expand_stack_space(FILE *fd, off_t off)
{
	if (off!=0)
		fprintf(fd, "\tsubq $%ld, %%rsp\n", off);
}

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

static char* get_next_call_register(enum reg_use r)
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
	if (f->do_inline) {
		current_func=f;
		doing_inline=true;
		current_arg=0;
		return;
	}
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
	if (doing_inline) {
		assign_var(fd, reg, current_func->arguments[current_arg]);
		current_arg++;
		return;
	}
	/*TODO: Fix this to work better. */	
	char *next=get_next_call_register(INT);
	char *name=get_reg_name(reg, pointer_size);

	if (strcmp(name, next))
		fprintf(fd, "\tmovq %s, %s\n", name, next);
}

void call(FILE *fd, struct func_t *f)
{

	if (f->do_inline) {
		generate_statement(fd, f->statement_list);
		doing_inline=false;
		return;
	}
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
	if (doing_inline) {
		return;
	}

	if (in_main && multiple_functions) {
		fprintf(fd, "\tleave\n\tret\n");
	} else {
		fprintf(fd, "\tmovq %%rbp, %%rsp\n");
		fprintf(fd, "\tpopq %%rbp\n");
		fprintf(fd, "\tret\n");
	}
}

off_t get_var_offset(struct statem_t *s, off_t current_off);
static off_t get_var_offset_expr(struct expr_t *e, off_t current_off)
{
	if (e==NULL)
		return 0;

	off_t o=0, coo=current_off+o;
	struct func_t *f;
	switch(e->kind) {
	case bin_op:
		o+=get_var_offset_expr(e->left, coo);
		o+=get_var_offset_expr(e->right, coo);
		break;

	case pre_un_op:
		o+=get_var_offset_expr(e->right, coo);
		break;

	case post_un_op:
		o+=get_var_offset_expr(e->left, coo);
		break;
	
	case arg:
		o+=get_var_offset_expr(e->right, coo);
		o+=get_var_offset_expr(e->attrs.argument, coo+o);
		break;

	case funccall:
		f=e->attrs.function;
		if (f->do_inline) {
			o+=get_var_offset(f->statement_list, coo);
			int x;
			for (x=0; x<f->num_arguments; x++) {
				o+=get_type_size(f->arguments[x]->type);
				f->arguments[x]->offset=-o;
			}
		}
		break;
	}

	return o;
}

off_t get_var_offset(struct statem_t *s, off_t current_off)
{
	off_t o=0;
	if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++)
			o+=get_var_offset(s->attrs.list.statements[x], current_off+o);
	} else if (s->kind==declare) {
		o=get_type_size(s->attrs.var->type);
		s->attrs.var->offset=-(o+current_off);
	} else if (s->kind==_if) {
		o+=get_var_offset(s->attrs._if.block, current_off+o);
	} else if (s->kind==_while) {
		o+=get_var_offset(s->attrs._while.block, current_off+o);
	} else if (s->kind==do_while) {
		o+=get_var_offset(s->attrs.do_while.block, current_off+o);
		o+=get_var_offset_expr(s->attrs.do_while.condition, current_off+o);
	} else if (s->kind==_for) {
		o+=get_var_offset(s->attrs._for.block, current_off+o);
	} else if (s->kind==expr) {
		o+=get_var_offset_expr(s->attrs.expr, current_off+o);
	}

	return o;
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

