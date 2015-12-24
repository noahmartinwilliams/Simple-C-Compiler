#ifndef __GENERATOR_TYPES_H__
#define __GENERATOR_TYPES_H__
#include "types.h"
#include <stdbool.h>

enum reg_use { RET, INT };
struct reg_size {
	char *name;
	size_t size;
};

struct reg_t {
	struct reg_size *sizes;
	int num_sizes;
	size_t size;
	bool in_use, used_for_call;
	int depth;
	enum reg_use use;
};

#endif
