#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/generator-globals.h"
#include "generator/generator-expr.h"
#include "globals.h"
#include "generator/generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"
#include "generator/generator-statem.h"

char* generate_global_string(FILE *fd, char *str)
{
	return make_global_string(fd, str);
}


void setup_types()
{
	num_types=4;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=malloc(sizeof(struct type_t));
	struct type_t *i=types[num_types-1];
	struct tbody_t *b=NULL;

	i->name=strdup("int");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	b=i->body;
	b->size=int_size;
	b->is_struct=false;
	b->refcount=1;
	b->is_union=false;
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

	types[num_types-4]=malloc(sizeof(struct type_t));
	i=types[num_types-4];
	i->name=strdup("size_t");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	b=i->body;
	b->size=sizeof(size_t);
	b->is_struct=false;
	b->refcount=1;
	b->is_union=false;
	i->refcount=1;
	i->native_type=true;
}

void setup_generator()
{
	setup_types();
	setup_backend();
}

void generate_function(FILE *fd, struct func_t *f)
{
	make_function(fd, f);
	generate_statement(fd, f->statement_list);
	return_from_call(fd);
}

char* prepare_var_assignment(FILE *fd, struct expr_t *dest)
{
	char *ret=NULL;
	if (dest->kind==var) {
		if (dest->attrs.var->scope_depth!=0) {
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
		s->attrs.var->scope_depth=0;
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
