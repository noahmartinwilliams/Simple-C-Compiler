#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include "generator/globals.h"
#include "generator/types.h"
#include "types.h"
#include "backend/int-arithmetic.h"
#include "backend/float-arithmetic.h"

typedef void (*func_t) (FILE *fd, struct reg_t *a, struct reg_t *b);

static void intern(FILE *fd, func_t _float, func_t _int, struct reg_t *a, struct reg_t *b)
{
	if (a->use==FLOAT || a->use==FLOAT_RET)
		_float(fd, a, b);
	else
		_int(fd, a, b);
}

#define macro(X) void X (FILE *fd, struct reg_t *a, struct reg_t *b)\
{\
	intern(fd, float_##X, int_##X, a, b);\
}

macro(add)
macro(sub)
macro(mul)
void _div(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	intern(fd, float_div, int_div, a, b);
}
