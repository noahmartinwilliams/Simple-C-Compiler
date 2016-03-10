arg_declaration: type_with_stars IDENTIFIER {
	struct arguments_t *a=malloc(sizeof(struct arguments_t));
	a->vars=calloc(1, sizeof(struct var_t*));
	a->vars[0]=malloc(sizeof(struct var_t));
	struct var_t *v=a->vars[0];
	init_var(v);
	v->name=$2;
	v->type=$1;
	v->type->refcount++;
	v->scope_depth=1;
	v->refcount=2;
	a->num_vars=1;
	add_var(v);
	$$=a;
} | arg_declaration ',' type_with_stars IDENTIFIER {
	struct arguments_t *a=$1;
	a->num_vars++;
	a->vars=realloc(a->vars, a->num_vars*sizeof(struct var_t*));
	int n=a->num_vars-1;
	a->vars[n]=malloc(sizeof(struct var_t));
	struct var_t *v=a->vars[n];
	init_var(v);
	v->scope_depth=1;
	v->refcount=2;
	v->name=$4;
	v->type=$3;
	v->type->refcount++;
	add_var(v);
	free_type($3);
	$$=a;
};

function_header: type_with_stars IDENTIFIER '(' ')' {
	$$=malloc(sizeof(struct func_t));
	init_func($$);
	$$->name=strdup($2);
	$$->ret_type=$1;
	$$->num_arguments=0;
	$$->arguments=NULL;
	$$->statement_list=NULL;
	free(current_function);
	current_function=strdup($$->name);
	free($2);
} | type_with_stars IDENTIFIER '(' arg_declaration ')' {
	$$=malloc(sizeof(struct func_t));
	$$->name=strdup($2);
	init_func($$);
	$$->ret_type=$1;
	$1->refcount++;
	$$->arguments=$4->vars;
	$$->num_arguments=$4->num_vars;
	free($4);
	free(current_function);
	current_function=$2;
	$$->statement_list=NULL;
} | EXTERN function_header {
	$2->attributes|=_extern;
	$$=$2;
} | type_with_stars IDENTIFIER '(' arg_declaration ',' MULTI_ARGS ')' {
	struct func_t *f=malloc(sizeof(struct func_t));
	init_func(f);
	f->name=$2;
	f->ret_type=$1;
	$1->refcount++;
	f->num_arguments=0;
	f->arguments=$4->vars;
	f->num_arguments=$4->num_vars;
	f->has_var_args=true;
	f->statement_list=NULL;
	free($4);
	free(current_function);
	current_function=strdup($2);
	$$=f;
} | STATIC function_header {
	$2->attributes|=_static;
	$$=$2;
} | INLINE function_header {
	$2->attributes|=_inline;
	$$=$2;
};

function: function_header '{' block '}' {
	$1->statement_list=$3;
	if (found_inline_in_function && $1->attributes&_inline){
		yyerror ("Inline functions must not contain other inline functions");
		exit(2);
	}
	found_inline_in_function=false;
	$$=$1;
}
