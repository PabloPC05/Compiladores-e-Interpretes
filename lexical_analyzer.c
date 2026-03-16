#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexical_analyzer.h"
#include "input_system.h"
#include "symbol_table.h"
#include "errors.h"
#include "definitions.h"

// Verifica si el caracter actual coincide con el esperado
static int match(int expected) {
    int c = nextChar();
    if (c == expected) return 1;
    if (c != EOF) returnChar();
    return 0;
}

// Comentario de linea: // ... \n
static void skipLineComment() {
    int c;
    do {
        c = nextChar();
    } while (c != '\n' && c != EOF);
}

// Comentario de bloque /* ... */
static void skipBlockComment() {
    int c;
    do {
        c = nextChar();
        // Consumimos caracteres hasta encontrar la secuencia de cierre "*/". Si encontramos EOF antes, reportamos error
    } while (c != EOF && !(c == '*' && match('/')));
    if (c == EOF)
        report(getLine(), getColumn(), ERR_EOF_BLOCK_COMMENT);
}

// Comentario anidado: /+ ... +/
static void skipNestedComment() {
    int depth = 1; // Profundidad de anidamiento
    int c; // Caracter actual
    while (depth > 0) { // Mientras haya comentarios abiertos
        c = nextChar(); // Leemos el siguiente caracter
        if (c == EOF) { // Si llegamos al final del archivo antes de cerrar todos los comentarios, reportamos error
            report(getLine(), getColumn(), ERR_EOF_NESTED_COMMENT);
            return;
        }
        // Si encontramos el inicio de un nuevo comentario anidado, incrementamos la profundidad
        if (c == '/' && match('+'))      depth++;
        // Si encontramos el final de un comentario, decrementamos la profundidad
        else if (c == '+' && match('/')) depth--;
    }
}

static void skipWhitespace() {
    int skipping = 1;

    // Mientras sigamos encontrando espacios o comentarios, seguimos saltando
    while (skipping) {
        // Actualizamos el bloque de inicio al bloque activo y colocamos el puntero de inicio en la posición del puntero de delantero para que el lexema que se forme a continuación sea correcto incluso si hemos tenido que cambiar de bloque
        moveInicio();
        int c = nextChar();

        // Si el caracter es un espacio seguimos saltando
        if (isspace(c)) continue;

        // Si estamos ante un posible comentario
        if (c == '/') {
            // Pedimos el siguiente caracter
            int next = nextChar();

            // Determinamos el tipo de caracter posible
            if (next == '/')      { skipLineComment();   }
            else if (next == '*') { skipBlockComment();  }
            else if (next == '+') { skipNestedComment(); }

            // Si no es ninguno de los tipos de comentarios posibles
            else {
                // Si el siguiente caracter no es EOF, lo devolvemos para que pueda ser procesado como parte de otro token
                if (next != EOF) returnChar();
                returnChar();

                // No estamos ante un comentario, por lo que dejamos de saltar
                skipping = 0;
            }
        } else {
            // Si tampoco había espacios que saltar ni comentarios, devolvemos el caracter para que pueda ser procesado como parte de otro token y dejamos de saltar
            if (c != EOF) returnChar();
            skipping = 0;
        }
    }
}

// Lee un identificador o palabra reservada
static Symbol readIdentifier() {
    int length = 1;
    int startCol = getColumn();

    int c = nextChar();
    while (isalnum(c) || c == '_') {
        length++;
        c = nextChar();
    }
    // Si el caracter que ha detenido la lectura no es EOF, lo devolvemos para que pueda ser procesado como parte de otro token
    if (c != EOF) returnChar();

    // Si el identificador supera el límite de longitud, reportamos error pero seguimos procesándolo para no perder más errores posibles
    if (length > MAX_LEXEME_LEN) {
        report(getLine(), startCol, ERR_ID_TOO_LONG);
    }

    // Buscamos el lexema en la tabla de símbolos. Si no está, se insertará con token IDENTIFICADOR. Devolvemos el componente léxico resultante
    char *lex = getLexeme();
    Symbol lc = lookupOrInsert(lex, IDENTIFIER);
    return makeSymbol(lc.token, lex);
}


// Consume digitos hexadecimales con separador '_'. Devuelve num de digitos reales.
static int consumeHex() {
    int n = 0;
    int c = nextChar();
    while (isxdigit(c) || c == '_') {
        if (isxdigit(c)) n++;
        c = nextChar();
    }
    if (c != EOF) returnChar();
    return n;
}

// Consume digitos binarios con separador '_'. Devuelve num de digitos reales.
static int consumeBin() {
    int n = 0;
    int c = nextChar();
    while (c == '0' || c == '1' || c == '_') {
        if (c != '_') n++;
        c = nextChar();
    }
    if (c != EOF) returnChar();
    return n;
}

// Consume digitos decimales con separador '_'
static void consumeDec() {
    int c = nextChar();
    while (isdigit(c) || c == '_') c = nextChar();
    if (c != EOF) returnChar();
}


// Intenta leer parte decimal (.digitos). Devuelve 1 si la leyo
static int readDecimalPart() {
    int c = nextChar();
    // Si no es un punto, devolvemos el caracter para que pueda ser procesado como parte de otro token
    if (c != '.') {
        if (c != EOF) returnChar();
        return 0;
    }

    // Si tenemos un punto, el siguiente caracter debe ser un digito para que sea parte decimal
    c = nextChar();
    if (isdigit(c)) {
        consumeDec();
        return 1;
    }

    // Si el siguiente caracter no es un dígito, debemos devolver el caracter leído para que pueda ser procesado como parte de otro token
    if (c != EOF) returnChar();
    returnChar();
    return 0;
}

// Intenta leer exponente (e/E [+-] digitos). Devuelve 1 si lo leyo
static int readExponent() {
    // Tomamos el siguiente caracter para ver si es 'e' o 'E'
    int c = nextChar();
    if (c != 'e' && c != 'E') {
        // Si no lo es y no es EOF, lo devolvemos para que pueda ser procesado como parte de otro token
        if (c != EOF) returnChar();
        return 0;
    }

    // Si tenemos 'e' o 'E', el siguiente caracter puede ser un signo opcional
    int next = nextChar();
    // Si es un signo
    if (next == '+' || next == '-') {
        // Si tras el signo hay digitos, consumimos el exponente y devolvemos 1
        int after = nextChar();
        if (isdigit(after)) {
            consumeDec();
            return 1;
        }
        // Si no hay digitos tras el signo debemos volver a atrás para que pueda ser procesado como parte de otro token
        if (after != EOF) returnChar();
        returnChar();   // retrocedemos el signo
        returnChar();   // retrocedemos la 'e' o 'E'
        return 0;
    }

    // Si tras la 'e' o 'E' hay digitos, consumimos el exponente y devolvemos 1
    if (isdigit(next)) {
        consumeDec();
        return 1;
    }

    // Si no hay digitos tras la 'e' o 'E', debemos devoler el caracter leído para que pueda ser procesado como parte de otro token
    if (next != EOF) returnChar();
    returnChar();   // retrocedemos la 'e' o 'E'
    return 0;
}

static Symbol readNumber(int first) {
    int isFloat = 0;

    if (first == '0') {
        int c = nextChar();

        if (c == 'x' || c == 'X') {

            // Si no han consumido dígitos hexadecimales tras el prefijo, reportamos error
            if (consumeHex() == 0) {
                report(getLine(), getColumn(), ERR_HEX_NO_DIGITS);
                return makeSymbol(TOKEN_ERROR, getLexeme());
            }
            return makeSymbol(LIT_INT, getLexeme());

        } else if (c == 'b' || c == 'B') {

            // Si no se ha consumido ningun digito binario tras el prefijo, reportamos error
            if (consumeBin() == 0) {
                report(getLine(), getColumn(), ERR_BIN_NO_DIGITS);
                return makeSymbol(TOKEN_ERROR, getLexeme());
            }
            return makeSymbol(LIT_INT, getLexeme());
        }

        if (c != EOF) returnChar();
    }

    // Decimal o flotante: devolver primero para que consumeDec lo relea
    returnChar();
    consumeDec();

    if (readDecimalPart()) isFloat = 1;
    if (readExponent()) isFloat = 1;

    if (isFloat)
        return makeSymbol(LIT_FLOAT, getLexeme());

    return makeSymbol(LIT_INT, getLexeme());
}

// Autómata para leer cadenas de caracteres (strings)
static Symbol readString() {
    int c = nextChar();

    while (c != '"' && c != EOF) {
        if (c == '\\') {
            int esc = nextChar();
            if (esc != 'n' && esc != 't' && esc != 'r' && esc != '\\' && esc != '"') {
                report(getLine(), getColumn(), ERR_UNKNOWN_ESCAPE);
            }
        }
        c = nextChar();
    }

    if (c == EOF) {
        report(getLine(), getColumn(), ERR_UNTERMINATED_STR);
        return makeSymbol(TOKEN_ERROR, getLexeme());
    }

    return makeSymbol(LIT_STRING, getLexeme());
}


static Symbol readOperator(int c) {
    switch (c) {
    case '/': return makeSymbol(match('=') ? OP_DIV_ASSIGN : '/',  getLexeme());
    case '=': return makeSymbol(match('=') ? OP_EQ  : '=',        getLexeme());
    case '!': return makeSymbol(match('=') ? OP_NEQ : '!',        getLexeme());
    case '&': return makeSymbol(match('&') ? OP_AND : '&',        getLexeme());
    case '|': return makeSymbol(match('|') ? OP_OR  : '|',        getLexeme());
    case '*': return makeSymbol(match('=') ? OP_MUL_ASSIGN : '*', getLexeme());

    case '<':
        if (match('=')) return makeSymbol(OP_LEQ, getLexeme());
        if (match('<')) return makeSymbol(OP_SHL, getLexeme());
        return makeSymbol('<', getLexeme());

    case '>':
        if (match('=')) return makeSymbol(OP_GEQ, getLexeme());
        if (match('>')) return makeSymbol(OP_SHR, getLexeme());
        return makeSymbol('>', getLexeme());

    case '+':
        if (match('+')) return makeSymbol(OP_INC,         getLexeme());
        if (match('=')) return makeSymbol(OP_PLUS_ASSIGN, getLexeme());
        return makeSymbol('+', getLexeme());

    case '-':
        if (match('-')) return makeSymbol(OP_DEC,          getLexeme());
        if (match('=')) return makeSymbol(OP_MINUS_ASSIGN, getLexeme());
        return makeSymbol('-', getLexeme());

    case '%':
        if (match('=')) return makeSymbol(OP_MOD_ASSIGN, getLexeme());
        return makeSymbol('%', getLexeme());

    case ';': case ',': case '.':
    case '(': case ')': case '[': case ']':
    case '{': case '}': case '~': case '^':
        return makeSymbol(c, getLexeme());

    default:
        report(getLine(), getColumn(), ERR_UNKNOWN_CHAR);
        return makeSymbol(TOKEN_ERROR, getLexeme());
    }
}


Symbol nextToken() {
    skipWhitespace();

    moveInicio();
    int c = nextChar();

    if (c == EOF)
        return makeSymbol(TOKEN_EOF, getLexeme());

    if (isalpha(c) || c == '_')
        return readIdentifier();

    if (isdigit(c))
        return readNumber(c);

    if (c == '"')
        return readString();

    return readOperator(c);
}

void freeToken(Symbol *lc) {
    free(lc->lexeme);
    lc->lexeme = NULL;
}
