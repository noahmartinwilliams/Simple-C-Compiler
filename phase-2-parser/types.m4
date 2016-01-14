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
	body->attrs.vars.num_vars=$4->attrs.list.num;
	body->attrs.vars.vars=calloc(body->attrs.vars.num_vars, sizeof(struct var_t*));
	size_t size=0;
	register int x;
	body->attrs.vars.alignment=word_size;
	struct statem_t **statements=$4->attrs.list.statements;
	size_t alignment=body->attrs.vars.alignment;
	for (x=0; x<$4->attrs.list.num; x++) {
		struct var_t *var=statements[x]->attrs.var;
		int s=get_type_size(var->type);
		if (s<alignment)
			size+=alignment;
		else
			size+=s;
		body->attrs.vars.vars[x]=var;
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
	type->body=malloc(sizeof(struct tbody_t));
	type->body->refcount=1;

	type->body->is_union=true;
	type->body->is_struct=false;

	type->body->attrs.vars.num_vars=0;
	type->body->attrs.vars.vars=NULL;
	register int x;
	size_t max_size=0;
	struct var_t **v=type->body->attrs.vars.vars;
	int num_vars=type->body->attrs.vars.num_vars;
	struct statem_t **statements=$4->attrs.list.statements;
	for (x=0; x<$4->attrs.list.num; x++) {
		size_t s=get_type_size($4->attrs.list.statements[x]->attrs.var->type);
		num_vars++;
		v=realloc(v, num_vars*sizeof(struct var_t*));
		v[num_vars-1]=malloc(sizeof(struct var_t));

		v[num_vars-1]=statements[x]->attrs.var;
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
