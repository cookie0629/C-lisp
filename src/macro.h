#ifndef MACRO_H
#define MACRO_H

#include "runtime/nucleus.h"

void macro_init();
int macro_is_definition(LispVal* expr);
void macro_register(LispVal* expr);

LispVal* macro_expand(LispVal* expr);

#endif