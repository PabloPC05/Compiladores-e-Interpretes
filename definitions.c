#include "definitions.h"

Symbol makeSymbol(int token, char *lexeme){
    Symbol s;
    s.token  = token;
    s.lexeme = lexeme;
    return s;
}
