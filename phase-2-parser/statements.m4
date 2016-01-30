statement_list: statement { 
	$$=malloc(sizeof(struct statem_t));
	$$->kind=list;
	$$->attrs.list.statements=calloc(1, sizeof(struct statem_t));
	$$->attrs.list.statements[0]=$1;
	$$->attrs.list.num=1;
} | statement_list statement {
	$1->attrs.list.num++;
	$1->attrs.list.statements=realloc($1->attrs.list.statements, $1->attrs.list.num*sizeof(struct statem_t*));
	$1->attrs.list.statements[$1->attrs.list.num-1]=$2;
	$$=$1;
};

statement: expression ';' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=expr;
	$$->attrs.expr=$1;
	$$->has_gotos=$1->has_gotos;
} | '{' statement_list '}' {
	$$=$2;
} | var_declaration | WHILE '(' expression ')' statement {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_while;
	$$->attrs._while.condition=$3;
	$$->attrs._while.block=$5;
	$$->has_gotos=$5->has_gotos;
} | RETURN expression ';' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=ret;
	$$->attrs.expr=$2;
	$$->has_gotos=false;
	/*TODO: Fix this to properly detect gotos from the epxression statements in expression. */
} | IF '(' expression ')' statement ELSE statement {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_if;
	$$->attrs._if.condition=$3;
	$$->attrs._if.block=$5;
	$$->attrs._if.else_block=$7;
	$$->has_gotos=$5->has_gotos || $7->has_gotos;
} | IF '(' expression ')' statement %prec IFX{
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_if;
	$$->has_gotos=$5->has_gotos;
	$$->attrs._if.condition=$3;
	$$->attrs._if.block=$5;
	$$->attrs._if.else_block=NULL;
} | BREAK ';' { 
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_break;
	$$->has_gotos=false;
} | CONTINUE ';' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_continue;
	$$->has_gotos=false;
} | IDENTIFIER ':' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=label;
	$$->attrs.label_name=strdup($1);
	$$->has_gotos=true;
	free($1);
} | GOTO IDENTIFIER ';' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_goto;
	$$->attrs.label_name=strdup($2);
	$$->has_gotos=true;
	free($2);
}  | for_loop | DO statement WHILE '(' expression ')' ';' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=do_while;
	$$->has_gotos=$2->has_gotos;
	$$->attrs.do_while.condition=$5;
	$$->attrs.do_while.block=$2;
} | SWITCH '(' expression ')' '{' switch_list '}' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_switch;
	$$->has_gotos=$6->has_gotos;
	$$->attrs._switch.cases=$6;
	$$->attrs._switch.tester=$3;
} | RETURN ';' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=ret;
	$$->attrs.expr=NULL;
	$$->has_gotos=false;
};

switch_list: switch_element {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=list;
	$$->attrs.list.num=1;
	$$->attrs.list.statements=calloc(1, sizeof(struct statem_t));
	$$->attrs.list.statements[0]=$1;
} | switch_list switch_element {
	$1->attrs.list.num++;
	$1->attrs.list.statements=realloc($1->attrs.list.statements, $1->attrs.list.num*sizeof(struct statem_t*));
	$1->attrs.list.statements[$1->attrs.list.num-1]=$2;
	$$=$1;
} ;

switch_element: CASE expression ':' statement_list {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_case;
	$$->attrs._case.block=$4;
	$$->attrs._case.condition=$2;
} | DEFAULT ':' statement_list {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_default;
	$$->attrs._default.def=$3;
} | CASE expression ':' {
	$$=malloc(sizeof(struct statem_t));
	$$->kind=_case;
	$$->attrs._case.block=NULL;
	$$->attrs._case.condition=$2;
}
