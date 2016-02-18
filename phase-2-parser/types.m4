type_with_stars: type | type_with_stars '*' {
	$$=increase_type_depth($1, 1);
};

type: TYPE {
	if ($1==NULL) {
		yyerror("type not known\n");
		exit(1);
	}
	free_type(current_type);
	current_type=$1;
	current_type->refcount+=2;
	$$=$1;
} | STRUCT IDENTIFIER '{' struct_var_declarations '}' {
	/* TODO: check to see if there's already a struct prototyped by this name, and use that instead if it exists. */
	struct type_t *type=malloc(sizeof(struct type_t));
	type->refcount=2;
	type->name=strdup($2);
	type->pointer_depth=0;
	type->body=malloc(sizeof(struct tbody_t));

	struct tbody_t *body=type->body;
	body->core_type=_INT;
	int y=0;
	struct statem_t *tmp=$4;
	for (; tmp!=NULL; tmp=tmp->right, y++) {}
	body->attrs.vars.num_vars=y;
	body->attrs.vars.vars=calloc(body->attrs.vars.num_vars, sizeof(struct var_t*));
	size_t size=0;
	body->attrs.vars.alignment=word_size;
	struct statem_t *current=$4;
	size_t alignment=body->attrs.vars.alignment;
	for (y=0; current!=NULL; current=current->right, y++) {
		assert(current->left->kind==declare);
		struct var_t *var=current->left->attrs._declare.var;
		int s=get_type_size(var->type);
		if (s<alignment)
			size+=alignment;
		else
			size+=s;
		body->attrs.vars.vars[y]=var;
		var->refcount=1;

	}

	/*TODO: free struct_var_declarations */

	body->size=size;
	body->is_struct=true;
	body->is_union=false;
	add_type(type);
	$$=type;
	current_type=type;
	free($2);
} | UNION IDENTIFIER '{' struct_var_declarations '}' {
	struct type_t *type=malloc(sizeof(struct type_t));
	type->refcount=2;
	type->pointer_depth=0;
	type->name=strdup($2);
	struct tbody_t *body=type->body=malloc(sizeof(struct tbody_t));
	body->core_type=_INT;
	body->refcount=1;

	body->is_union=true;
	body->is_struct=false;

	body->attrs.vars.num_vars=0;
	body->attrs.vars.vars=NULL;
	register int x;
	size_t max_size=0;
	struct var_t **v=body->attrs.vars.vars;
	int num_vars=body->attrs.vars.num_vars;
	struct statem_t *current=$4;
	for (; current!=NULL; current=current->right) {
		size_t s=get_type_size(current->left->attrs._declare.var->type);
		num_vars++;
		v=realloc(v, num_vars*sizeof(struct var_t*));
		v[num_vars-1]=malloc(sizeof(struct var_t));

		v[num_vars-1]=current->left->attrs._declare.var;
		v[num_vars-1]->refcount=1;
		if (s>=max_size)
			max_size=s;
	}
	type->body->attrs.vars.num_vars=num_vars;
	type->body->attrs.vars.vars=v;

	type->body->size=max_size;
	current_type=type;
	current_type->refcount++;
	add_type(type);
	free($2);
	$$=type;
} | STRUCT IDENTIFIER {
	free_type(current_type);
	$$=current_type=get_struct_by_name($2);
	$$->refcount+=2;
	free($2);
};
