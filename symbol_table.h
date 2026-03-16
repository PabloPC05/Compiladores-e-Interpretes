#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "definitions.h"

void initSymbolTable();
Symbol lookupOrInsert(const char *lexeme, int newToken);
void printSymbolTable();
void freeSymbolTable();

#endif /* SYMBOL_TABLE_H */
