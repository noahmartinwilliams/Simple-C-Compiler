stars: %empty {
	$$=0;
} | stars '*' {
	$$=$1 + 1;
} ;

struct_var_declarations: type IDENTIFIER ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=list;
	s->attrs.list.num=1;
	s->attrs.list.statements=malloc(sizeof(struct statem_t*));
	s->attrs.list.statements[0]=malloc(sizeof(struct statem_t*));

	struct statem_t *var=s->attrs.list.statements[0];
	var->kind=declare;
	var->attrs._declare.var=malloc(sizeof(struct var_t));
	var->attrs._declare.expr=NULL;

	struct var_t *v=var->attrs._declare.var;
	v->refcount=1;

	v->type=$1;
	$1->refcount++;
	v->refcount=1;
	v->scope_depth=scope_depth;
	v->name=strdup($2);
	free($2);
	$$=s;
} | struct_var_declarations type_with_stars IDENTIFIER ';' {
	$1->attrs.list.num++;
	int num_statements=$1->attrs.list.num;
	struct statem_t **statements=$1->attrs.list.statements;
	$1->attrs.list.statements=realloc($1->attrs.list.statements, num_statements*sizeof(struct statem_t*));
	statements=$1->attrs.list.statements;

	statements[num_statements-1]=malloc(sizeof(struct statem_t));
	statements[num_statements-1]->kind=declare;
	statements[num_statements-1]->attrs._declare.var=malloc(sizeof(struct var_t));
	statements[num_statements-1]->attrs._declare.expr=NULL;

	struct var_t *v=statements[num_statements-1]->attrs._declare.var;

	v->name=strdup($3);
	v->type=$2;
	$2->refcount++;
	v->scope_depth=scope_depth;
	v->refcount=1;
	free($3);
	$$=$1;
};

var_declaration: var_declaration_start ';' {
	$$=$1;
} | type_with_stars '(' stars IDENTIFIER ')' '(' arg_declaration ')' ';' {
	struct var_t *v=malloc(sizeof(struct var_t));
	struct type_t *t=v->type=malloc(sizeof(struct type_t));
	struct tbody_t *tb=t->body=malloc(sizeof(struct tbody_t));
	tb->size=pointer_size;
	tb->is_func_pointer=true;

	tb->refcount=t->refcount=v->refcount=1;
	tb->attrs.func_ptr.return_type=$1;
	t->pointer_depth=$3;
	t->native_type=false;

	v->name=strdup($4);
	free($4);

	t->body->is_struct=t->body->is_union=false;

	t->body->attrs.func_ptr.arguments=calloc($7->num_vars, $7->num_vars*sizeof(struct type_t));

	int x;
	for (x=0; x<$7->num_vars; x++) {
		t->body->attrs.func_ptr.arguments[x]=$7->vars[x]->type;
		$7->vars[x]->type->refcount++;
		free_var($7->vars[x]);
	}

	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	declaration->attrs._declare.var=v;
	declaration->attrs._declare.expr=NULL;
	v->scope_depth=scope_depth;
	v->hidden=false;
	add_var(v);
	$$=declaration;
};

var_declaration_start: type_with_stars IDENTIFIER {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=list;
	$$->attrs.list.num=1;
	$$->attrs.list.statements=calloc(1, sizeof(struct statem_t*));
	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	struct var_t *v=declaration->attrs._declare.var=malloc(sizeof(struct var_t));

	v->type=$1;
	$1->refcount++;
	v->name=strdup($2);
	free($2);
	v->refcount=2;
	v->scope_depth=scope_depth;
	v->hidden=false;
	declaration->attrs._declare.expr=NULL;
	add_var(v);
	$$->attrs.list.statements[0]=declaration;
} | type_with_stars IDENTIFIER '=' noncomma_expression { 
	$$=malloc(sizeof(struct statem_t));
	$$->kind=list;
	$$->attrs.list.num=1;
	$$->attrs.list.statements=calloc(1, sizeof(struct statem_t*));
	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	struct var_t *v;
	v=malloc(sizeof(struct var_t));

	v->name=strdup($2);
	free($2);
	v->scope_depth=scope_depth;
	v->hidden=false;
	v->refcount=4;
	add_var(v);
	v->type=$1;
	$1->refcount++;
	declaration->attrs._declare.var=v;
	declaration->attrs._declare.expr=$4;

	$$->attrs.list.statements[0]=declaration;

} | var_declaration_start ',' stars IDENTIFIER '=' noncomma_expression {
	$$=$1;
	($$->attrs.list.num)++;
	$$->attrs.list.statements=realloc($$->attrs.list.statements, $$->attrs.list.num*sizeof(struct statem_t*));
	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	struct var_t *v=malloc(sizeof(struct var_t));

	v->name=strdup($4);
	free($4);

	v->scope_depth=scope_depth;
	v->hidden=false;
	v->type=increase_type_depth(current_type, $3);
	v->type->refcount++;
	v->refcount=2;
	add_var(v);

	declaration->attrs._declare.var=v;
	declaration->attrs._declare.expr=$6;
	$$->attrs.list.statements[$$->attrs.list.num-1]=declaration;

} | var_declaration_start ',' stars IDENTIFIER {
	$$=$1;
	$$->attrs.list.num++;
	$$->attrs.list.statements=realloc($$->attrs.list.statements, $$->attrs.list.num*sizeof(struct statem_t*));
	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	struct var_t *v=malloc(sizeof(struct var_t));

	v->name=strdup($4);
	free($4);

	v->scope_depth=scope_depth;
	v->hidden=false;
	v->type=increase_type_depth(current_type, $3);
	v->refcount=2;
	add_var(v);

	declaration->attrs._declare.var=v;
	declaration->attrs._declare.expr=NULL;
	$$->attrs.list.statements[$$->attrs.list.num-1]=declaration;
}; 
