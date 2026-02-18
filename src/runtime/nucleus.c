#include "nucleus.h"
#include <string.h>

#ifdef _MSC_VER
    #define strdup _strdup
#endif

// Initialize the runtime (Placeholder for GC setup)
void lisp_runtime_init(size_t heap_size) {
    printf("[Runtime] Initializing heap with size: %zu bytes\n", heap_size);
}

// Shutdown the runtime
void lisp_runtime_shutdown() {
    // TODO: Free all allocated memory (if not using OS cleanup)
}

// Basic allocator (Placeholder: uses malloc directly)
LispVal* lisp_alloc(LispType type) {
    LispVal* val = (LispVal*)malloc(sizeof(LispVal));
    if (!val) {
        fprintf(stderr, "Fatal: Out of memory\n");
        exit(1);
    }
    val->type = type;
    val->marked = false;
    return val;
}

// --- Constructors ---

LispVal* lisp_int(long long v) {
    LispVal* val = lisp_alloc(LISP_INT);
    val->data.int_val = v;
    return val;
}

LispVal* lisp_float(double v) {
    LispVal* val = lisp_alloc(LISP_FLOAT);
    val->data.float_val = v;
    return val;
}

LispVal* lisp_symbol(const char* s) {
    LispVal* val = lisp_alloc(LISP_SYMBOL);
    val->data.str_val = strdup(s); // Copy string
    return val;
}

LispVal* lisp_string(const char* s) {
    LispVal* val = lisp_alloc(LISP_STRING);
    val->data.str_val = strdup(s); // Copy string
    return val;
}

LispVal* lisp_cons(LispVal* car, LispVal* cdr) {
    LispVal* val = lisp_alloc(LISP_CONS);
    val->data.cons.car = car;
    val->data.cons.cdr = cdr;
    return val;
}

// --- Printer ---

void lisp_print(LispVal* val) {
    if (!val) {
        printf("NULL");
        return;
    }
    switch (val->type) {
        case LISP_INT: 
            printf("%lld", val->data.int_val); 
            break;
        case LISP_FLOAT: 
            printf("%f", val->data.float_val); 
            break;
        case LISP_SYMBOL: 
            printf("%s", val->data.str_val); 
            break;
        case LISP_STRING: 
            printf("\"%s\"", val->data.str_val); 
            break;
        case LISP_NIL: 
            printf("NIL"); 
            break;
        case LISP_T: 
            printf("T"); 
            break;
        case LISP_CONS:
            printf("(");
            lisp_print(val->data.cons.car);
            // Handle rest of the list
            LispVal* curr = val->data.cons.cdr;
            while (curr && curr->type == LISP_CONS) {
                printf(" ");
                lisp_print(curr->data.cons.car);
                curr = curr->data.cons.cdr;
            }
            // Dotted list handling (if last cdr is not NIL)
            if (curr && curr->type != LISP_NIL) {
                printf(" . ");
                lisp_print(curr);
            }
            printf(")");
            break;
    }
}