stars: '*' {
	$$=1;
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
	var->attrs.var=malloc(sizeof(struct var_t));

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
	$1->attrs.list.num++;
	int num_statements=$1->attrs.list.num;
	struct statem_t **statements=$1->attrs.list.statements;
	$1->attrs.list.statements=realloc($1->attrs.list.statements, num_statements*sizeof(struct statem_t*));
	statements=$1->attrs.list.statements;

	statements[num_statements-1]=malloc(sizeof(struct statem_t));
	statements[num_statements-1]->kind=declare;
	statements[num_statements-1]->attrs.var=malloc(sizeof(struct var_t));

	struct var_t *v=statements[num_statements-1]->attrs.var;

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
	declaration->attrs.var=v;
	v->scope_depth=scope_depth;
	v->hidden=false;
	add_var(v);
	$$=declaration;
};

var_declaration_start: type_with_stars IDENTIFIER {
	struct statem_t *declaration=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	struct var_t *v=declaration->attrs.var=malloc(sizeof(struct var_t));

	v->type=$1;
	$1->refcount++;
	v->name=strdup($2);
	free($2);
	v->refcount=2;
	v->scope_depth=scope_depth;
	v->hidden=false;
	add_var(v);
	$$=declaration;
} | type_with_stars IDENTIFIER '=' expression { 
	struct statem_t *block=malloc(sizeof(struct statem_t));
	struct statem_t *declaration;
	struct expr_t *assignment;
	struct var_t *v;
	block->attrs.list.statements=calloc(2, sizeof(struct statem_t*));
	block->attrs.list.num=2;
	block->attrs.list.statements[1]=malloc(sizeof(struct statem_t));
	block->attrs.list.statements[1]->kind=expr;
	assignment=block->attrs.list.statements[1]->attrs.expr=malloc(sizeof(struct expr_t));
	declaration=block->attrs.list.statements[0]=malloc(sizeof(struct statem_t));
	block->kind=list;

	declaration->kind=declare;


	assignment->kind=bin_op;
	assignment->attrs.bin_op=strdup("=");
	assignment->right=$4;
	assignment->type=$1;

	assignment->left=malloc(sizeof(struct expr_t));
	assignment->left->kind=var;
	assignment->left->left=assignment->left->right=NULL;
	assignment->left->type=$1;
	$1->refcount+=2;

	v=declaration->attrs.var=assignment->left->attrs.var=malloc(sizeof(struct var_t));

	v->name=strdup($2);
	free($2);
	v->scope_depth=scope_depth;
	v->hidden=false;
	v->refcount=4;
	add_var(v);
	v->type=$1;
	$1->refcount++;
	$$=block;

} | var_declaration_start ',' IDENTIFIER '=' expression {
	struct statem_t *block=malloc(sizeof(struct statem_t));
	block->kind=list;

	block->attrs.list.num=3;
	struct statem_t **statements=block->attrs.list.statements=calloc(3, sizeof(struct statem_t*));

	statements[0]=$1;
	struct statem_t *declaration=statements[1]=malloc(sizeof(struct statem_t));
	declaration->kind=declare;
	struct expr_t *holder=malloc(sizeof(struct expr_t));
	holder->left=holder->right=NULL;
	holder->type=$5->type;
	$5->type->refcount++;
	holder->kind=var;
	struct var_t *v=holder->attrs.var=declaration->attrs.var=malloc(sizeof(struct var_t));
	v->name=strdup($3);
	free($3);

	v->scope_depth=scope_depth;
	v->hidden=false;
	v->type=current_type;
	current_type->refcount++;
	v->refcount=4;

	statements[1]=declaration;
	add_var(v);

	struct statem_t *assignment=statements[2]=malloc(sizeof(struct statem_t));
	assignment->kind=expr;

	struct expr_t *expr=assignment->attrs.expr=malloc(sizeof(struct expr_t));
	expr->kind=bin_op;
	expr->attrs.bin_op=strdup("=");
	expr->left=holder;
	expr->right=$5;
	expr->type=$5->type;
	$5->type->refcount++;

	$$=block;
} | var_declaration_start ',' IDENTIFIER {
	struct statem_t *block=malloc(sizeof(struct statem_t));
	block->kind=list;
	block->attrs.list.statements=calloc(2, sizeof(struct statem_t*));
	block->attrs.list.statements[1]=malloc(sizeof(struct statem_t));
	block->attrs.list.statements[1]->kind=declare;
	block->attrs.list.num=2;
	struct var_t *v=block->attrs.list.statements[1]->attrs.var=malloc(sizeof(struct var_t));
	block->attrs.list.statements[0]=$1;
	v->name=strdup($3);
	free($3);
	v->refcount=2;
	v->type=current_type;
	current_type->refcount++;

	v->scope_depth=scope_depth;
	v->hidden=false;
	add_var(v);
	$$=block;
} | var_declaration_start ',' stars IDENTIFIER {
	struct statem_t *block=malloc(sizeof(struct statem_t));
	block->kind=list;
	block->attrs.list.num=2;
	block->attrs.list.statements=calloc(2, sizeof(struct statem_t*));
	block->attrs.list.statements[0]=$1;
	block->attrs.list.statements[1]=malloc(sizeof(struct statem_t));

	struct statem_t *declaration=block->attrs.list.statements[1];

	declaration->kind=declare;

	struct var_t *v=malloc(sizeof(struct var_t));
	v->type=increase_type_depth(current_type, $3);
	v->name=strdup($4);
	free($4);

	v->scope_depth=scope_depth;
	v->hidden=false;
	v->refcount=2;

	add_var(v);

	declaration->attrs.var=v;
	$$=block;
} | var_declaration_start ',' stars IDENTIFIER '=' expression {
	struct statem_t *block=malloc(sizeof(struct statem_t));
	block->kind=list;

	block->attrs.list.num=3;
	block->attrs.list.statements=calloc(3, sizeof(struct statem_t*));

	block->attrs.list.statements[0]=$1;

	struct statem_t *declaration=block->attrs.list.statements[1]=malloc(sizeof(struct statem_t));
	declaration->kind=declare;

	struct var_t *v=malloc(sizeof(struct var_t));
	v->type=increase_type_depth(current_type, $3);
	v->type->refcount++;
	v->refcount=2;

	v->name=strdup($4);
	free($4);

	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=v->type;
	v->type->refcount++;

	e->left=setup_var_expr(v);
	e->right=$6;
	e->kind=bin_op;
	e->attrs.bin_op=strdup("=");

	declaration->attrs.var=v;

	struct statem_t *expression=block->attrs.list.statements[2]=malloc(sizeof(struct statem_t));
	expression->kind=expr;
	expression->attrs.expr=e;
	$$=block;
} ;
