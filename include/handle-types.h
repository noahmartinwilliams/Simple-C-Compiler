#ifndef __HANDLE_TYPES_H__
#define __HANDLE_TYPES_H__


extern struct type_t* get_type_by_name(char *name);


extern void free_type(struct type_t *t);
extern void free_tbody(struct tbody_t *t);
extern int get_type_index_by_name(char *name);
#endif
