#ifndef __PRINTER_H__
#define __PRINTER_H__
#include <stddef.h>
#include "print-tree.h"
#include "parser/exprs.h"
#include "parser/statems.h"


static inline void print_s(struct statem_t *s)
{
	print_statem(" ", s);
}

static inline void print_f(struct func_t *f)
{
	printf("function: %s\n", f->name);
	print_s(f->statement_list);
}

#endif
