#include "types.h"
#include <stdbool.h>
#include "optimizations.h"

#ifdef OPTIMIZE_IF_CONSTANT_CONDITION
bool optimize_if_constant_condition=true;
#endif

#ifdef OPTIMIZE_WHILE_CONSTANT_CONDITION
bool optimize_while_constant_condition=true;
#endif

#ifdef OPTIMIZE_FOR_CONSTANT_CONDITION
bool optimize_for_constant_condition=true;
#endif
