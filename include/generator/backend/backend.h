#ifndef __BACKEND_H__
#define __BACKEND_H__
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "generator/generator-types.h"
#include "generator/backend/registers.h"
#include "generator/backend/jumps.h"
#include "generator/backend/agnostic-arithmetic.h"
#include "generator/backend/statics.h"
#include "generator/backend/int-arithmetic.h"
#include "generator/backend/calls.h"
#include "generator/backend/variables.h"

extern void cleanup_backend();
extern void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void assign_constant_int(FILE *fd, int e);
extern void place_comment(FILE *fd, char *str);
extern void setup_backend();

#endif
