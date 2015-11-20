#ifndef __STACK_H__
#define __STACK_H__
#include <stdlib.h>

struct stack_t {
	void *element;
	struct stack_t *next;
};

static inline void push_intern(struct stack_t **s, void *element)
{
	struct stack_t *tmp=*s;
	*s=malloc(sizeof(struct stack_t));
	(*s)->next=tmp;
	(*s)->element=element;
}

static inline void* pop_intern(struct stack_t **s)
{
	struct stack_t *tmp=(*s)->next;
	void *ptr=(*s)->element;
	free(*s);
	*s=tmp;
	return ptr;
}

#define push(S, X) push_intern(&S, X)
#define pop(S) pop_intern(&S)

#endif
