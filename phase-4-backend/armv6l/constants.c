#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities/types.h"
#include "generator/types.h"
#include "generator/generator.h"
#include "generator/globals.h"
#include "backend/agnostic-arithmetic.h"
#include "backend/int-arithmetic.h"
#include "backend/variables.h"
#include "backend/registers.h"
#include "backend/calls.h"
#include "backend/jumps.h"
#include "globals.h"
#include "types.h"
#include "backend/globals.h"

static inline void size_error(char *message, size_t size)
{
	fprintf(stderr, "Internal Error: unknown size: %ld passed to %s\n", size, message);
	exit(1);
}

void assign_constant(FILE *fd, struct expr_t *e)
{
	fprintf(fd, "\tmov r0, #%ld\n", e->attrs.cint_val);
}

void assign_constant_int(FILE *fd, long int e)
{
	fprintf(fd, "\tmov r0, #%ld\n", e);
}

