#ifndef __GLOBALS_H__
#define __GLOBALS_H__
#include "types.h"
#include <stdbool.h>

extern int current_line;


extern int current_char;
extern char *current_file;
extern struct type_t **types;
extern int num_types;
extern bool evaluate_constants;
#endif
