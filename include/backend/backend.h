#ifndef __BACKEND_H__
#define __BACKEND_H__
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "generator/types.h"
#include "backend/globals.h"
#include "backend/registers.h"
#include "backend/jumps.h"
#include "backend/agnostic-arithmetic.h"
#include "backend/statics.h"
#include "backend/int-arithmetic.h"
#include "backend/calls.h"
#include "backend/variables.h"

extern void cleanup_backend();
extern void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void assign_constant_int(FILE *fd, int e);
extern void place_comment(FILE *fd, char *str);
extern void setup_backend();
extern void assign_constant(FILE *fd, struct expr_t *e);

#endif
