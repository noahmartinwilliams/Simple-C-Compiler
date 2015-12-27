#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator-globals.h"
#include "generator-expr.h"
#include "globals.h"
#include "backend.h"
#include "generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"
#include "generator-statem.h"

char* generate_global_string(FILE *fd, char *str)
{
	return make_global_string(fd, str);
}

char* prepare_var_assignment(FILE *fd, struct expr_t *dest);

void get_address(FILE *fd, struct expr_t *_var)
{
	place_comment(fd, "& (");
	depth++;
	struct reg_t *ret=get_ret_register(_var->type->body->size);
	if (_var->kind==var) {
		fprintf(fd, "\tmovq %%rbp, %s\n", get_reg_name(ret, pointer_size));
		inc_by_int(fd, _var->attrs.var->offset, get_reg_name(ret, pointer_size), pointer_size);
	}
	depth--;
	place_comment(fd, "(");
}

void setup_types()
{
	num_types=3;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=malloc(sizeof(struct type_t));
	struct type_t *i=types[num_types-1];
	i->name=strdup("int");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=int_size;
	i->body->is_struct=false;
	i->body->refcount=1;
	i->body->is_union=false;
	i->refcount=1;
	i->native_type=true;

	types[num_types-2]=malloc(sizeof(struct type_t));
	i=types[num_types-2];
	i->name=strdup("char");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=char_size;
	i->body->is_struct=false;
	i->body->refcount=1;
	i->body->is_union=false;
	i->refcount=1;
	i->native_type=true;

	types[num_types-3]=malloc(sizeof(struct type_t));
	i=types[num_types-3];
	i->name=strdup("void");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=0;
	i->body->is_struct=false;
	i->body->refcount=1;
	i->body->is_union=false;
	i->refcount=1;
	i->native_type=true;
}

void setup_generator()
{
	setup_types();
	setup_registers();
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
	} else if (s->kind==_for) {
		o+=get_var_offset(s->attrs._for.block, current_off+o);
	}

	return o;
}

void generate_function(FILE *fd, struct func_t *f)
{
	fprintf(fd, "\t.text\n\t.globl %s\n\t.type %s, @function\n%s:\n", f->name, f->name, f->name);
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

char* prepare_var_assignment(FILE *fd, struct expr_t *dest)
{
	char *ret=NULL;
	if (dest->kind==var) {
		if (dest->attrs.var->scope!=0) {
			asprintf(&ret, "%ld(%%rbp)", dest->attrs.var->offset);
		}
	}
	return ret;
}

void generate_global_vars(FILE *fd, struct statem_t *s)
{
	if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++) {
			generate_global_vars(fd, s->attrs.list.statements[x]);
		}
	} else if (s->kind==declare) {
		backend_make_global_var(fd, s->attrs.var);
		s->attrs.var->scope=0;
	}
}
