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

    /* Build free list */
    for (size_t i = 0; i < heap_capacity - 1; i++) {
        heap[i].data.cons.cdr = &heap[i + 1];
    }
    heap[heap_capacity - 1].data.cons.cdr = NULL;

    free_list = &heap[0];
}

void gc_shutdown(void) {
    if (!heap) return;

    /* Free remaining string data */
    for (size_t i = 0; i < heap_capacity; i++) {
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
        gc_collect();
        if (!free_list) {
            fprintf(stderr, "GC: Out of memory\n");
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

    for (size_t i = 0; i < heap_capacity; i++) {
        if (heap[i].marked) {
            heap[i].marked = false;
        } else {
            /* Free string memory if needed */
            if ((heap[i].type == LISP_STRING || heap[i].type == LISP_SYMBOL) &&
                heap[i].data.str_val) {
                free(heap[i].data.str_val);
                heap[i].data.str_val = NULL;
            }

            /* Add to free list */
            heap[i].data.cons.cdr = free_list;
            free_list = &heap[i];
        }
    }
}

//GC Entry Point
void gc_collect(void) {
    /* Mark from roots */
    for (size_t i = 0; i < root_top; i++) {
        mark(*root_stack[i]);
    }

    /* Sweep unreachable objects */
    sweep();
}