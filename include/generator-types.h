#ifndef __GENERATOR_TYPES_H__
#define __GENERATOR_TYPES_H__
#include "types.h"
#include <stdbool.h>

/* TODO: figure out how to work with the fact that the x86 architecture
 * has registers that each can have multiple sizes 
 * possibly give each register a list of "other" registers that it is mutually
 * exclusive with. */
struct reg_t {
	char *name;
	char *pushable_name;
	size_t size;
	bool in_use;
	int depth;
	enum { RET, INT } use;
};

#endif
