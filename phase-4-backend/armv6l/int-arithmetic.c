#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/globals.h"
#include "generator/types.h"
#include "generator/generator.h"
#include "globals.h"
#include "types.h"
#include "backend/backend.h"
#include "backend/registers.h"

void int_num(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		if (a->use!=RET && b->use!=RET) {
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(a));
			fprintf(fd, "\tidivl %s\n",  reg_name(b));
		} else if (a->use!=RET && b->use==RET) {
			fprintf(fd, "\tpushq %%rdx\n");
			fprintf(fd, "\tpushq %%rax\n");
			fprintf(fd, "\tpushq %%rbx\n");
			if (strcmp(reg_name(a), "%ebx")) {
				fprintf(fd, "\tmovl %%eax, %%ebx\n");
				fprintf(fd, "\tmovl %s, %%eax\n", reg_name(a));
			} else {
				fprintf(fd, "\tpushq %%rcx\n");
				fprintf(fd, "\tmovl %%ebx, %%ecx\n");
				fprintf(fd, "\tmovl %%eax, %%ebx\n");
				fprintf(fd, "\tmovl %%ecx, %%eax\n");
				fprintf(fd, "\tpopq %%rcx\n");
			}
			fprintf(fd, "\tcltd\n");
			fprintf(fd, "\tidivl %%ebx\n");
			fprintf(fd, "\tpopq %%rbx\n");
			fprintf(fd, "\tpopq %%rax\n");
			fprintf(fd, "\tmovl %%edx, %%eax\n");
			fprintf(fd, "\tpopq %%rdx\n");
			/* T.T my eyes are bleeding. */
		} else if (a->use==RET && b->use!=RET) {
			fprintf(fd, "\tidivl %s\n", reg_name(b));
		}
	} else {
		fprintf(stderr, "Internal error: int_num passed size: %ld. No size handler found.\n", a->size);
		exit(1);
	}
}

void int_add(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\tadd %s, %s, %s\n", reg_name(b), reg_name(b), reg_name(a));
	fprintf(fd, "\tmov r0, %s\n", reg_name(b));
}

void int_sub(FILE *fd, struct reg_t *b, struct reg_t *a)
{
	if (a->size==char_size) {
		fprintf(fd, "\tsubb %s, %s\n", reg_name(a), reg_name(b));
		fprintf(fd, "\tmovb %s, %%al\n", reg_name(b));
	} else if (a->size==word_size) {
		fprintf(fd, "\tsubl %s, %s\n", reg_name(a), reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
	} else if (a->size==pointer_size) {
		fprintf(fd, "\tsubq %s, %s\n", reg_name(a), reg_name(b));
		if (b->use!=RET)
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
	} else {
		fprintf(stderr, "Internal Error: unknown size: %ld passed to int_inc\n", r->size);
		exit(1);
	}
}

void int_dec(FILE *fd, struct reg_t *r)
{
	if (r->size==word_size) {
		fprintf(fd, "\tdecl %s\n", reg_name(r));
		if (r->use!=RET) 
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(r));
	} else {
		fprintf(stderr, "Internal Error: unknown size: %ld passed to int_dec\n", r->size);
		exit(1);
	}
}

void int_neg(FILE *fd, struct reg_t *r)
{
	size_t s=r->size;
	if (r->use!=RET) {
		if (s==char_size) {
			fprintf(fd, "\tnegb %s\n", reg_name(r));
			fprintf(fd, "\tmovb %s, %%al\n", reg_name(r));
		} else if (s==word_size) {
			fprintf(fd, "\tnegl %s\n", reg_name(r));
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(r));
		} else if (s==pointer_size) {
			fprintf(fd, "\tnegq %s\n", reg_name(r));
			fprintf(fd, "\tmovq %s, %%rax\n", reg_name(r));
		} else {
			fprintf(fd, "Internal error: %d size passed to int_neg\n", s);
			exit(1);
		}
	} else {
		if (s==char_size)
			fprintf(fd, "\tnegb %s\n", reg_name(r));
		else if (s==word_size)
			fprintf(fd, "\tnegl %s\n", reg_name(r));
		else if (s==pointer_size)
			fprintf(fd, "\tnegq %s\n", reg_name(r));
		else {
			fprintf(fd, "Internal error: %d size passed to int_neg\n", s);
			exit(1);
		}
	}
}

void convert_int_size(FILE *fd, struct reg_t *r, size_t new_size, bool is_signed)
{
	struct reg_t *ret=get_ret_register(new_size, false);
	if (r->size < new_size && is_signed) {
		assign_reg(fd, r, ret);
		char *conv_instr=calloc(4, sizeof(char));
		conv_instr[0]='c';
		conv_instr[2]='t';
		if (r->size==char_size)
			conv_instr[1]='b';
		if (r->size==short_size)
			conv_instr[1]='w';
		if (r->size==word_size)
			conv_instr[1]='l';
		if (r->size==long_size)
			conv_instr[1]='q';

		if (new_size==char_size)
			conv_instr[3]='b';
		if (new_size==short_size)
			conv_instr[3]='w';
		if (new_size==word_size)
			conv_instr[3]='l';
		if (new_size==long_size)
			conv_instr[3]='q';
		fprintf(fd, "\t%s\n", conv_instr);
			
	} else if (r->size < new_size && !is_signed) {
		assign_constant_int(fd, 0);
		set_register_size(r, new_size);
		assign_reg(fd, r, ret);
	} else {
		set_register_size(ret, new_size);
		set_register_size(r, new_size);
		assign_reg(fd, r, ret);
	}
}
