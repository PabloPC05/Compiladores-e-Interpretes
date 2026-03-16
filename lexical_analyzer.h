#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include "definitions.h"

Symbol nextToken();
void freeToken(Symbol *lc);

#endif /* LEXICAL_ANALYZER_H */
