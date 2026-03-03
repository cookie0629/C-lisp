#ifndef BUILDER_H
#define BUILDER_H

#include <stddef.h>

// A dynamic string buffer for generating C code
typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} CodeBuilder;

// Initialize a new builder
CodeBuilder* builder_create();

// Free the builder
void builder_free(CodeBuilder* cb);

// Append a formatted string to the builder (like printf)
void builder_append(CodeBuilder* cb, const char* format, ...);

// Get the raw C string
const char* builder_get(CodeBuilder* cb);

#endif // BUILDER_H