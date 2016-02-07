#include "types.h"
#include "stack.h"

size_t current_stack_offset=0;
size_t word_size=4;
size_t int_size=4;
size_t byte_size=8;
size_t pointer_size=8;
size_t char_size=1;
size_t long_size=8;
size_t float_size=sizeof(float);

/* Architecture specific globals start here. */
bool last_comparison_float=false;
