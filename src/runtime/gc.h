#ifndef GC_H
#define GC_H

#include "nucleus.h"
#include <stddef.h>

/* --- GC Public API --- */

void gc_init(size_t object_count);
void gc_shutdown(void);

LispVal* gc_alloc(LispType type);

void gc_collect(void);

/* Heap statistics */
size_t gc_used_count(void);   /* number of live (allocated) objects */
size_t gc_free_count(void);   /* number of objects on the free list */
size_t gc_capacity(void);     /* total heap capacity */
void   gc_print_stats(void);  /* print a one-line summary to stdout */

/* Root stack management */
void gc_push(LispVal** slot);
void gc_pop(void);

#define GC_PUSH(v) gc_push(&(v))
#define GC_POP()   gc_pop()

#endif