#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/generator-expr.h"
#include "globals.h"
#include "generator/types.h"
#include "parser/types.h"
#include "parser/funcs.h"
#include "stack.h"
#include "generator/generator-statem.h"

char* generate_global_string(FILE *fd, char *str)
{
	return make_global_string(fd, str);
}


void setup_types()
{
	num_types=5;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=malloc(sizeof(struct type_t));
	struct type_t *i=types[num_types-1];
	struct tbody_t *b=NULL;

	i->name=strdup("int");
	init_type(i);
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=int_size;

	types[num_types-2]=malloc(sizeof(struct type_t));
	i=types[num_types-2];
	init_type(i);
	i->name=strdup("char");
	i->is_signed=false;
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=char_size;

	types[num_types-3]=malloc(sizeof(struct type_t));
	i=types[num_types-3];
	init_type(i);
	i->name=strdup("void");
	i->body=NULL;

	types[num_types-4]=malloc(sizeof(struct type_t));
	i=types[num_types-4];
	init_type(i);
	i->name=strdup("float");
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=float_size;
	b->core_type=_FLOAT;

	types[num_types-5]=malloc(sizeof(struct type_t));
	i=types[num_types-5];
	init_type(i);
	i->name=strdup("long");
	b=i->body=malloc(sizeof(struct tbody_t));
	init_body(b);
	b->size=long_size;
}

void setup_generator()
{
	setup_backend();
	setup_types();
}

void generate_function(FILE *fd, struct func_t *f)
{
	make_function(fd, f);
	generate_statement(fd, f->statement_list);
	return_from_call(fd);
	prepare_for_new_function(fd);
}

char* prepare_var_assignment(FILE *fd, struct expr_t *dest)
{
	char *ret=NULL;
	if (dest->kind==var)
		if (dest->attrs.var->scope_depth!=0)
			asprintf(&ret, "%ld(%%rbp)", dest->attrs.var->offset);
	return ret;
}

void generate_global_vars(FILE *fd, struct statem_t *s)
{
	if (s->kind==block)
		for (; s!=NULL ; s=s->right)
			generate_global_vars(fd, s->left);
	else if (s->kind==declare) {
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
	if (s->left!=NULL)
		o+=get_var_offset(s->left, current_off+o);
	if (s->right!=NULL)
		o+=get_var_offset(s->right, current_off+o);
	if (s->expr!=NULL)
		o+=get_var_offset_expr(s->expr, current_off+o);
	if (s->kind==declare) {
		int x;
		o=get_type_size(s->attrs.var->type);
		if (s->attrs.var->type->num_arrays!=0) {
			size_t offset=get_type_size(s->attrs.var->type);
			for (x=0; x<s->attrs.var->type->num_arrays; x++) {
				if (s->attrs.var->type->array_dimensions[x]==0)
					offset=pointer_size;
				else
					offset*=s->attrs.var->type->array_dimensions[x];
			}
			offset+=pointer_size;
			o+=offset;

		}
		s->attrs.var->offset=-(o+current_off);
	} else if (s->kind==_for) {
		o+=get_var_offset_expr(s->attrs._for.initial, current_off+o);
		o+=get_var_offset_expr(s->attrs._for.update, current_off+o);
	}

	return o;
}
