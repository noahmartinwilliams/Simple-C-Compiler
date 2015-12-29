#ifndef __OPTIMIZATION_GLOBALS_H__
#define __OPTIMIZATION_GLOBALS_H__
#include "optimizations.h"

#ifdef OPTIMIZE_IF_CONSTANT_CONDITION
extern bool optimize_if_constant_condition;
#endif

#ifdef OPTIMIZE_WHILE_CONSTANT_CONDITION
extern bool optimize_while_constant_condition;
#endif

#ifdef OPTIMIZE_FOR_CONSTANT_CONDITION
extern bool optimize_for_constant_condition;
#endif

#ifdef OPTIMIZE_DO_WHILE_CONSTANT_CONDITION
extern bool optimize_do_while_constant_condition;
#endif

#endif
