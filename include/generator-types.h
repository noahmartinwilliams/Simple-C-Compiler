#ifndef __GENERATOR_TYPES_H__
#define __GENERATOR_TYPES_H__
#include "types.h"
#include <stdbool.h>

struct reg_size {
	char *name;
	size_t size;
};
struct reg_t {
	struct reg_size *sizes;
	int num_sizes;
	size_t size;
	bool in_use;
	int depth;
	enum { RET, INT } use;
};

#endif
