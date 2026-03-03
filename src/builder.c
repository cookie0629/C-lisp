#include "builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

CodeBuilder* builder_create() {
    CodeBuilder* cb = (CodeBuilder*)malloc(sizeof(CodeBuilder));
    cb->capacity = 256;
    cb->length = 0;
    cb->data = (char*)malloc(cb->capacity);
    cb->data[0] = '\0';
    return cb;
}

void builder_free(CodeBuilder* cb) {
    if (cb) {
        free(cb->data);
        free(cb);
    }
}

void builder_append(CodeBuilder* cb, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Determine required length
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        va_end(args);
        return; 
    }

    // Resize if necessary
    if (cb->length + needed + 1 > cb->capacity) {
        while (cb->length + needed + 1 > cb->capacity) {
            cb->capacity *= 2;
        }
        cb->data = (char*)realloc(cb->data, cb->capacity);
    }

    // Actually write the formatted string
    vsprintf(cb->data + cb->length, format, args);
    cb->length += needed;
    
    va_end(args);
}

const char* builder_get(CodeBuilder* cb) {
    return cb->data;
}