#ifndef __PRINTER_H__
#define __PRINTER_H__
#include <stddef.h>
#include "print-tree.h"
#include "handle-exprs.h"
#include "handle-statems.h"

inline void print_e(struct expr_t *e)
{
	print_tree((__printer_function_t) &print_expr, e, "", offsetof(struct expr_t, left), offsetof(struct expr_t, right));
}

inline void print_s(struct statem_t *s)
{
	print_statem(" ", s);
}

inline void print_f(struct func_t *f)
{
	printf("function: %s\n", f->name);
	print_s(f->statement_list);
}

#endif
