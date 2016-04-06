#ifndef __BACKEND_ERRORS_H__
#define __BACKEND_ERRORS_H__
#include <stdio.h>

static inline void s_error(const char *fun, int line, size_t size)
{
	fprintf(stderr, "Internal error: in %s, line: %d : size: %zd passed, no size handler found.\n", fun, line, size);
	exit(1);
}

#define size_error(X) s_error( __func__, __LINE__, X)

#endif
