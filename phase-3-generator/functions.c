#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/expr.h"
#include "globals.h"
#include "utilities/types.h"
#include "utilities/funcs.h"
#include "stack.h"
#include "generator/statem.h"

void generate_function(FILE *fd, struct func_t *f)
{
	make_function(fd, f);
	generate_statement(fd, f->statement_list);
	return_from_call(fd);
	prepare_for_new_function(fd);
}
