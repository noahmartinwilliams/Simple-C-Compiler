#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator-globals.h"
#include "generator-types.h"
#include "generator.h"
#include "globals.h"
#include "types.h"

void int_add(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==char_size) {
		fprintf(fd, "\taddb %s, %s\n", get_reg_name(a, word_size), get_reg_name(b, word_size));
		fprintf(fd, "\tmovb %s, %%al\n", get_reg_name(b, word_size));
	} else if (a->size==word_size) {
		fprintf(fd, "\taddl %s, %s\n", get_reg_name(a, word_size), get_reg_name(b, word_size));
		fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, word_size));
	} else if (a->size==pointer_size) {
		fprintf(fd, "\taddq %s, %s\n", get_reg_name(a, pointer_size), get_reg_name(b, pointer_size));
		fprintf(fd, "\tmovq %s, %%rax\n", get_reg_name(b, pointer_size));
	}
}

void int_inc_by(FILE *fd, struct reg_t *a, char *dest)
{
	if (a->size==char_size)
		fprintf(fd, "\taddb %s, %s\n", reg_name(a), dest);
	else if (a->size==word_size)
		fprintf(fd, "\taddl %s, %s\n", reg_name(a), dest);
	else if (a->size==pointer_size)
		fprintf(fd, "\taddq %s, %s\n", reg_name(a), dest);

}

void inc_by_int(FILE *fd, int i, char *dest, size_t size)
{
	if (size==char_size)
		fprintf(fd, "\taddb $%d, %s\n", i, dest);
	else if (size==word_size)
		fprintf(fd, "\taddl $%d, %s\n", i, dest);
	else if (size==pointer_size)
		fprintf(fd, "\taddq $%d, %s\n", i, dest);
}

void int_sub(FILE *fd, struct reg_t *b, struct reg_t *a)
{
	if (a->size==char_size) {
		fprintf(fd, "\tsubb %s, %s\n", reg_name(a), reg_name(b));
		fprintf(fd, "\tmovb %s, %%al\n", reg_name(b));
	} else if (a->size==word_size) {
		fprintf(fd, "\tsubl %s, %s\n", reg_name(a), reg_name(b));
		fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
	} else if (a->size==pointer_size) {
		fprintf(fd, "\tsubq %s, %s\n", reg_name(a), reg_name(b));
		fprintf(fd, "\tmovq %s, %%rax\n", reg_name(b));
	}
}

void int_div(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	/* The X86 handles integer division in some stupid way.
	For some reason the div instruction takes just one argument, and
	DX:AX is the numerator, and the argument is the denominator.
	This code has to be complicated to deal with that nonsense. */
	if (b->use!=RET && a->use!=RET)
		fprintf(fd, "\tmovl %s, %%eax\n", reg_name(a));
	else if (b->use==RET) {
		fprintf(fd, "\tpushq %%rbx\n");
		fprintf(fd, "\tmovl %%eax, %%ebx\n");
		fprintf(fd, "\tcltd\n");
		fprintf(fd, "\tidivl %%ebx\n");
		fprintf(fd, "\tmovl %%ebx, %%eax\n");
		fprintf(fd, "\tpopq %%rbx\n");
	} else {
		fprintf(fd, "\tcltd\n");
		fprintf(fd, "\tidivl %s\n", reg_name(b));
	}

	/* TBH I don't even know what this means. I just copied this
	from the GNU C compiler's output. */
}

void int_mul(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	/* The nice people at Intel need to learn to be more F*cking consistent. D:< */

	if (a->size==word_size) {
		if (b->use!=RET && a->use==RET)
			fprintf(fd, "\tmull %s\n", reg_name(b));
		else if (b->use==RET && a->use!=RET)
			fprintf(fd, "\tmull %s\n", reg_name(a));
		else if (b->use!=RET && a->use!=RET) {
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
			fprintf(fd, "\tmull %s\n", reg_name(a));
			fprintf(fd, "\tmovl %%eax, %s\n", reg_name(b));
		}
	} else if (a->size==pointer_size)
		fprintf(fd, "\tmulq %s\n", reg_name(b));

}

void int_inc(FILE *fd, struct reg_t *r)
{
	if (r->size==word_size) {
		fprintf(fd, "\tincl %s\n", reg_name(r));
		if (r->use!=RET) 
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(r));
	}
}
