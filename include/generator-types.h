#ifndef __GENERATOR_TYPES_H__
#define __GENERATOR_TYPES_H__
#include "types.h"
#include <stdbool.h>

struct reg_t {
	char *name;
	size_t size;
	bool in_use;
	enum { RET, INT } use;
};

#endif
