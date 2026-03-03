#ifndef GC_H
#define GC_H

#include "nucleus.h"
#include <stddef.h>

/* --- GC Public API --- */

void gc_init(size_t object_count);
void gc_shutdown(void);

LispVal* gc_alloc(LispType type);

void gc_collect(void);

/* Root stack management */
void gc_push(LispVal** slot);
void gc_pop(void);

#define GC_PUSH(v) gc_push(&(v))
#define GC_POP()   gc_pop()

#endif