#ifndef NUCLEUS_H
#define NUCLEUS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* --- Type System Definitions --- */

typedef enum {
    LISP_INT,
    LISP_FLOAT,
    LISP_SYMBOL,
    LISP_STRING,
    LISP_CONS,
    LISP_NIL,
    LISP_T
} LispType;

// Forward declaration
struct LispVal;

// Cons Cell structure (Linked List Node)
typedef struct {
    struct LispVal *car;
    struct LispVal *cdr;
} ConsCell;

// Generic Lisp Object (Tagged Union)
typedef struct LispVal {
    LispType type;          // Type tag
    bool marked;            // GC mark bit
    
    union {
        long long int_val;  // Integer value
        double float_val;   // Float value
        char *str_val;      // String or Symbol name
        ConsCell cons;      // Cons pair (List)
    } data;
} LispVal;

/* --- Memory Management API --- */

// Initialize runtime environment (heap allocation)
void lisp_runtime_init(size_t heap_size);

// Shutdown runtime (cleanup)
void lisp_runtime_shutdown();

// Allocate a raw Lisp object (GC trigger point)
LispVal* lisp_alloc(LispType type);

/* --- Constructors --- */

LispVal* lisp_int(long long v);
LispVal* lisp_float(double v);
LispVal* lisp_symbol(const char* s);
LispVal* lisp_string(const char* s);
LispVal* lisp_cons(LispVal* car, LispVal* cdr);

/* --- Utilities --- */

// Print LispVal to stdout (for debugging)
void lisp_print(LispVal* val);

/* --- Logic & List Helpers --- */

// Check if a Lisp value is "truthy" (anything except NIL)
bool lisp_is_truthy(LispVal* val);

// Convert C boolean to Lisp T or NIL
LispVal* lisp_bool(bool b);

// Safe list operations
LispVal* lisp_car(LispVal* list);
LispVal* lisp_cdr(LispVal* list);

#endif // NUCLEUS_H