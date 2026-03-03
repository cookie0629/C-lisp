#ifndef PARSER_H
#define PARSER_H

#include "../runtime/nucleus.h"

/*
 * Parses a Lisp source code string into a list of expressions.
 * Returns a LispVal of type LISP_CONS (a list of top-level expressions)
 * or LISP_NIL if empty.
 */ 
LispVal* lisp_parse(const char* input);

/*
 * Reads a file content and parses it.
 */
LispVal* lisp_parse_file(const char* filename);

#endif // PARSER_H