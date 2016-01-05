
for_loop:  FOR '(' ';' ';' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_for;
	s->attrs._for.initial=s->attrs._for.cond=NULL;
	s->attrs._for.update=$5;
	s->attrs._for.block=$7;
	s->has_gotos=s->attrs._for.block->has_gotos;
	$$=s;
} | FOR '(' ';' expression ';' ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_for;
	s->attrs._for.initial=s->attrs._for.update=NULL;
	s->attrs._for.cond=$4;
	s->attrs._for.block=$7;
	s->has_gotos=s->attrs._for.block->has_gotos;
	$$=s;
} | FOR '(' ';' expression ';' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_for;
	s->attrs._for.initial=NULL;
	s->attrs._for.cond=$4;
	s->attrs._for.update=$6;
	s->attrs._for.block=$8;
	s->has_gotos=s->attrs._for.block->has_gotos;
	$$=s;
} | FOR '(' expression ';' ';' ')' statement { 
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_for;
	s->attrs._for.initial=$3;
	s->attrs._for.cond=s->attrs._for.update=NULL;
	s->attrs._for.block=$7;
	s->has_gotos=s->attrs._for.block->has_gotos;
	$$=s;
} | FOR '(' expression ';' ';' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));

	s->kind=_for;
	s->attrs._for.initial=$3;
	s->attrs._for.cond=malloc(sizeof(struct expr_t));

	struct expr_t *c=s->attrs._for.cond;
	c->kind=const_int;
	c->attrs.cint_val=1;
	c->left=NULL;
	c->right=NULL;
	c->type=get_type_by_name("int");
	c->type->refcount++;

	s->attrs._for.update=$6;
	s->attrs._for.block=$8;
	s->has_gotos=s->attrs._for.block->has_gotos;
	$$=s;
} | FOR '(' expression ';' expression ';' ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_for;
	s->attrs._for.initial=$3;
	s->attrs._for.cond=$5;
	s->attrs._for.update=NULL;
	s->attrs._for.block=$8;
	s->has_gotos=s->attrs._for.block->has_gotos;
	$$=s;
} | FOR '(' expression ';' expression ';' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_for;
	s->attrs._for.initial=$3;
	s->attrs._for.cond=$5;
	s->attrs._for.update=$7;
	s->attrs._for.block=$9;
	s->has_gotos=s->attrs._for.block->has_gotos;
	$$=s;
}; 
