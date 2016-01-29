#ifndef __GENERATOR_TYPES_H__
#define __GENERATOR_TYPES_H__
#include "types.h"
#include <stdbool.h>
#include "stack.h"

enum reg_use { RET, INT, FLOAT };
struct reg_size {
	char *name;
	size_t size;
};

struct reg_t {
	struct reg_size *sizes;
	int num_sizes;
	size_t size;
	bool in_use, used_for_call;
	struct stack_t *depths;
	enum reg_use use;
};

#endif
