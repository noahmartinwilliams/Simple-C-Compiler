stars: %empty {
	$$=0;
} | stars '*' {
	$$=$1 + 1;
} ;

struct_var_declarations: type IDENTIFIER ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	init_statem(s);
	s->kind=block;
	s->left=malloc(sizeof(struct statem_t));
	init_statem(s->left);

	struct statem_t *var=s->left;
	var->kind=declare;
	var->attrs.var=malloc(sizeof(struct var_t));
	init_var(var->attrs.var);
	var->expr=NULL;

	struct var_t *v=var->attrs.var;
	v->refcount=1;

	v->type=$1;
	$1->refcount++;
	v->refcount=1;
	v->scope_depth=scope_depth;
	v->name=strdup($2);
	free($2);
	$$=s;
} | struct_var_declarations type_with_stars IDENTIFIER ';' {
	$$=$1;
	struct statem_t *latest=$1;
	for (; latest->right!=NULL; latest=latest->right) {}

	latest->right=malloc(sizeof(struct statem_t));
	struct statem_t *new=latest->right;
	init_statem(new);
	new->kind=block;
	new->left=malloc(sizeof(struct statem_t));
	init_statem(new->left);
	new->left->kind=declare;
	new=new->left;
	new->attrs.var=malloc(sizeof(struct var_t));
	init_var(new->attrs.var);
	new->expr=NULL;

	struct var_t *v=new->attrs.var;

	v->name=$3;
	v->type=$2;
	$2->refcount++;
	v->scope_depth=scope_depth;
	v->refcount=1;
};

var_declaration: REGISTER var_declaration_start ';' {
	struct statem_t *s=$2;
	for (; s!=NULL; s=s->right)
		make_register_variable(s->left->attrs.var);
	$$=$2;
} | CONST var_declaration_start ';' {
	$$=$2;
	read_const_keyword=false;
} | var_declaration_start ';' {
	$$=$1;
} | type_with_stars '(' stars IDENTIFIER ')' '(' arg_declaration ')' ';' {
	struct var_t *v=malloc(sizeof(struct var_t));
	init_var(v);
	struct type_t *t=v->type=malloc(sizeof(struct type_t));
	struct tbody_t *tb=t->body=malloc(sizeof(struct tbody_t));
	tb->size=pointer_size;
	tb->is_func_pointer=true;
	tb->attrs.func_ptr.has_var_args=false;

	tb->refcount=t->refcount=v->refcount=1;
	tb->attrs.func_ptr.return_type=$1;
	t->pointer_depth=$3;
	t->native_type=false;

	v->name=strdup($4);
	free($4);

	t->body->kind=_normal;
	t->body->is_func_pointer=true;

	t->body->attrs.func_ptr.arguments=calloc($7->num_vars, $7->num_vars*sizeof(struct type_t));

	int x;
	for (x=0; x<$7->num_vars; x++) {
		t->body->attrs.func_ptr.arguments[x]=$7->vars[x]->type;
		$7->vars[x]->type->refcount++;
		free_var($7->vars[x]);
	}

	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	init_statem(declaration);
	declaration->kind=declare;
	declaration->attrs.var=v;
	v->scope_depth=scope_depth;
	v->hidden=false;
	add_var(v);
	$$=declaration;
};

var_declaration_start: type_with_stars IDENTIFIER {
	if (read_const_keyword)
		yyerror("constant not set at declaration");
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=block;
	struct statem_t *declaration=$$->left=malloc(sizeof(struct statem_t));
	init_statem(declaration);
	declaration->kind=declare;
	struct var_t *v=declaration->attrs.var=malloc(sizeof(struct var_t));
	init_var(v);

	v->type=$1;
	$1->refcount++;
	v->name=$2;
	v->refcount=2;
	v->scope_depth=scope_depth;
	v->hidden=false;
	add_var(v);
} | type_with_stars IDENTIFIER '=' noncomma_expression { 
	if (read_const_keyword) {
		add_constant($2, scope_depth, $4);
		$$=NULL;
	} else {
		$$=malloc(sizeof(struct statem_t));
		init_statem($$);
		$$->kind=block;
		$$->right=NULL;
		struct statem_t *declaration=$$->left=malloc(sizeof(struct statem_t));
		init_statem(declaration);
		declaration->kind=declare;
		struct var_t *v;
		v=malloc(sizeof(struct var_t));
		init_var(v);

		v->name=$2;
		v->refcount=4;
		add_var(v);
		v->type=$1;
		$1->refcount++;
		declaration->attrs.var=v;
		declaration->expr=$4;
	}

} | var_declaration_start ',' stars IDENTIFIER '=' noncomma_expression {
	if (read_const_keyword) {
		$$=NULL;
		add_constant($4, scope_depth, $6);
	} else {
		$$=$1;
		struct statem_t *s=$$;
		for (; s->right!=NULL; s=s->right) {}
		s->right=malloc(sizeof(struct statem_t));
		init_statem(s->right);
		s=s->right;
		init_statem(s);
		s->kind=block;
		struct statem_t *declaration=s->left=malloc(sizeof(struct statem_t));
		init_statem(declaration);
		declaration->kind=declare;
		struct var_t *v=malloc(sizeof(struct var_t));
		init_var(v);

		v->name=$4;

		v->type=increase_type_depth(current_type, $3);
		v->type->refcount++;
		v->refcount=2;
		add_var(v);

		declaration->attrs.var=v;
		declaration->expr=$6;
	}
} | var_declaration_start ',' stars IDENTIFIER {
	if (read_const_keyword) 
		yyerror("constant not set at declaration");
	$$=$1;
	struct statem_t *s=$$;
	for (; s->right!=NULL; s=s->right) {}
	s->right=malloc(sizeof(struct statem_t));
	init_statem(s->right);
	s=s->right;
	init_statem(s);
	s->kind=block;
	struct statem_t *declaration=s->left=malloc(sizeof(struct statem_t));
	init_statem(declaration);
	declaration->kind=declare;
	struct var_t *v=malloc(sizeof(struct var_t));
	init_var(v);

	v->name=$4;

	v->type=increase_type_depth(current_type, $3);
	v->refcount=2;
	add_var(v);

	declaration->attrs.var=v;
}; 
