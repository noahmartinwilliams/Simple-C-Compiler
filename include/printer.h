#ifndef __PRINTER_H__
#define __PRINTER_H__

#include "print-tree.h"
#include "print-stuff.h"
#include <stddef.h>

inline void print_e(struct expr_t *e)
{
	print_tree(&print_expr, e, "", offsetof(struct expr_t, left), offsetof(struct expr_t, right));
}

inline void print_s(struct statem_t *s)
{
	print_statem(" ", s);
}

#endif
