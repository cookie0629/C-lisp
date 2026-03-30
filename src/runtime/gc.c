#include "gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Heap Layout
static LispVal* heap = NULL;
static size_t heap_capacity = 0;

/* Free list implemented using LispVal memory */
static LispVal* free_list = NULL;

//Root Stack
#define GC_STACK_MAX 4096

static LispVal** root_stack[GC_STACK_MAX];
static size_t root_top = 0;

//Internal Helpers
static void mark(LispVal* obj);
static void sweep(void);

//Initialization
void gc_init(size_t object_count) {
    heap_capacity = object_count;

    heap = (LispVal*)malloc(sizeof(LispVal) * heap_capacity);
    if (!heap) {
        fprintf(stderr, "GC: Failed to allocate heap\n");
        exit(1);
    }

    /* Build free list and mark all slots as LISP_FREE */
    for (size_t i = 0; i < heap_capacity - 1; i++) {
        heap[i].type = LISP_FREE;
        heap[i].data.cons.cdr = &heap[i + 1];
    }
    heap[heap_capacity - 1].type = LISP_FREE;
    heap[heap_capacity - 1].data.cons.cdr = NULL;

    free_list = &heap[0];
}

void gc_shutdown(void) {
    if (!heap) return;

    /* Free remaining string data */
    for (size_t i = 0; i < heap_capacity; i++) {
        if (heap[i].type == LISP_FREE) continue;
        if ((heap[i].type == LISP_STRING || heap[i].type == LISP_SYMBOL) &&
            heap[i].data.str_val) {
            free(heap[i].data.str_val);
        }
    }

    free(heap);
    heap = NULL;
}

//Root Stack
void gc_push(LispVal** slot) {
    if (root_top >= GC_STACK_MAX) {
        fprintf(stderr, "GC: Root stack overflow\n");
        exit(1);
    }
    root_stack[root_top++] = slot;
}

void gc_pop(void) {
    if (root_top == 0) {
        fprintf(stderr, "GC: Root stack underflow\n");
        exit(1);
    }
    root_top--;
}

//Allocation
LispVal* gc_alloc(LispType type) {
    if (!free_list) {
        size_t before = gc_used_count();
        printf("[GC] Heap full (%zu objects). Triggering collection...\n", before);
        gc_collect();
        size_t after = gc_used_count();
        printf("[GC] Collection done. Reclaimed %zu objects. Used: %zu / %zu\n",
               before - after, after, heap_capacity);
        if (!free_list) {
            fprintf(stderr, "[GC] Out of memory after collection\n");
            exit(1);
        }
    }

    LispVal* obj = free_list;
    free_list = free_list->data.cons.cdr;

    obj->type = type;
    obj->marked = false;

    return obj;
}

//Mark Phase
static void mark(LispVal* obj) {
    if (!obj) return;
    if (obj->marked) return;
    if (obj->type == LISP_FREE) return;  /* never mark a free slot */

    obj->marked = true;

    switch (obj->type) {
        case LISP_CONS:
            mark(obj->data.cons.car);
            mark(obj->data.cons.cdr);
            break;

        case LISP_INT:
        case LISP_FLOAT:
        case LISP_STRING:
        case LISP_SYMBOL:
        case LISP_NIL:
        case LISP_T:
            /* No references inside */
            break;
    }
}

//Sweep Phase
static void sweep(void) {
    free_list = NULL;
    size_t reclaimed = 0;

    for (size_t i = 0; i < heap_capacity; i++) {
        /* Skip slots that were never allocated */
        if (heap[i].type == LISP_FREE) {
            heap[i].data.cons.cdr = free_list;
            free_list = &heap[i];
            continue;
        }

        if (heap[i].marked) {
            heap[i].marked = false;
        } else {
            /* --- DEBUG: log every reclaimed object --- */
#ifdef GC_DEBUG
            switch (heap[i].type) {
                case LISP_INT:
                    printf("[GC][sweep] reclaim INT      = %lld  (addr=%p)\n",
                           heap[i].data.int_val, (void*)&heap[i]);
                    break;
                case LISP_FLOAT:
                    printf("[GC][sweep] reclaim FLOAT    = %f  (addr=%p)\n",
                           heap[i].data.float_val, (void*)&heap[i]);
                    break;
                case LISP_STRING:
                    printf("[GC][sweep] reclaim STRING   = \"%s\"  (addr=%p)\n",
                           heap[i].data.str_val ? heap[i].data.str_val : "",
                           (void*)&heap[i]);
                    break;
                case LISP_SYMBOL:
                    printf("[GC][sweep] reclaim SYMBOL   = %s  (addr=%p)\n",
                           heap[i].data.str_val ? heap[i].data.str_val : "",
                           (void*)&heap[i]);
                    break;
                case LISP_CONS:
                    printf("[GC][sweep] reclaim CONS     (car=%p cdr=%p)  (addr=%p)\n",
                           (void*)heap[i].data.cons.car,
                           (void*)heap[i].data.cons.cdr,
                           (void*)&heap[i]);
                    break;
                case LISP_NIL:
                    printf("[GC][sweep] reclaim NIL      (addr=%p)\n", (void*)&heap[i]);
                    break;
                case LISP_T:
                    printf("[GC][sweep] reclaim T        (addr=%p)\n", (void*)&heap[i]);
                    break;
                case LISP_CLOSURE:
                    printf("[GC][sweep] reclaim CLOSURE  (addr=%p)\n", (void*)&heap[i]);
                    break;
                case LISP_THUNK:
                    printf("[GC][sweep] reclaim THUNK    (addr=%p)\n", (void*)&heap[i]);
                    break;
                default: break;
            }
#endif /* GC_DEBUG */

            /* Free string memory if needed */
            if ((heap[i].type == LISP_STRING || heap[i].type == LISP_SYMBOL) &&
                heap[i].data.str_val) {
                free(heap[i].data.str_val);
                heap[i].data.str_val = NULL;
            }

            /* Mark as free and add to free list */
            heap[i].type = LISP_FREE;
            heap[i].data.cons.cdr = free_list;
            free_list = &heap[i];
            reclaimed++;
        }
    }

#ifdef GC_DEBUG
    printf("[GC][sweep] --- total reclaimed: %zu objects ---\n", reclaimed);
#endif
}

//GC Entry Point
void gc_collect(void) {
#ifdef GC_DEBUG
    size_t before = gc_used_count();
    printf("[GC] ========== Collection START  (live before=%zu) ==========\n", before);
#endif

    /* Mark from roots */
    for (size_t i = 0; i < root_top; i++) {
        mark(*root_stack[i]);
    }

    /* Sweep unreachable objects */
    sweep();

#ifdef GC_DEBUG
    size_t after = gc_used_count();
    printf("[GC] ========== Collection END    (live after=%zu, reclaimed=%zu) ==========\n",
           after, before - after);
#endif
}

/* --- Heap Statistics --- */

size_t gc_free_count(void) {
    /* Count slots on the free list (type == LISP_FREE) */
    size_t n = 0;
    for (LispVal* p = free_list; p; p = p->data.cons.cdr) n++;
    return n;
}

size_t gc_capacity(void) { return heap_capacity; }

size_t gc_used_count(void) { return heap_capacity - gc_free_count(); }

void gc_print_stats(void) {
    size_t free_n = gc_free_count();
    size_t used_n = heap_capacity - free_n;
    printf("[GC] capacity=%zu  used=%zu  free=%zu  (%.1f%% used)\n",
           heap_capacity, used_n, free_n,
           heap_capacity ? 100.0 * used_n / heap_capacity : 0.0);
}