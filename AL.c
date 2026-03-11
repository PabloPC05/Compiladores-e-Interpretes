#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "AL.h"
#include "SI.h"
#include "TS.h"
#include "errores.h"
#include "definiciones.h"

#define MAX_LONGITUD_ID TAM_BUFFER

static int linea_actual   = 1;
static int columna_actual = 1;

static char leer_char(void) {
    char c = sig_caracter();
    if (c == '\n') { 
        linea_actual++; 
        columna_actual = 1; 
    } else { columna_actual++; }
    return c;
}

static void devolver_char(void) {
    devolver_caracter();
    if (columna_actual > 1) columna_actual--;
}

/*static char peek(void) {
    return peek_caracter();
}*/

static ComponenteLexico make_cl(int token, char *lexema) {
    ComponenteLexico cl;
    cl.token  = token;
    cl.lexema = lexema;
    return cl;
}

static int match(char esperado) {
    //if (peek() != esperado) return 0;
    // definimos match sin usar peek() para evitar problemas de sincronización con el buffer
    char c = leer_char();
    if (c != esperado) {
        devolver_char();
        return 0;
    }
    return 1;
}

// Comentario de linea: // ... \n */
static void saltar_comentario_linea(void) {
    leer_char(); // consumir segunda '/'
    char c;
    do {
        c = leer_char();
    } while (c != '\n' && c != (char)EOF);
}

// Comentario de bloque: /* ... */
static void saltar_comentario_bloque(void) {
    leer_char(); // consumir '*'
    char c, prev = 0;
    do {
        c = leer_char();
        if (c == (char)EOF) {
            fprintf(stderr, "Error [%d:%d]: fin de fichero dentro de comentario '/*'\n", linea_actual, columna_actual);
            return;
        }
        if (prev == '*' && c == '/') return;
        prev = c;
    } while (c != (char)EOF);
}

// Comentario anidado: /+ ... +/ 
static void saltar_comentario_anidado(void) {
    leer_char(); // consumir '+'
    int  depth = 1;
    char c, prev = 0;
    while (depth > 0) {
        c = leer_char();
        if (c == (char)EOF) {
            fprintf(stderr, "Error [%d:%d]: fin de fichero dentro de comentario '/+'\n", linea_actual, columna_actual);
            return;
        }
        if (prev == '/' && c == '+') { depth++; c = 0; }
        else if (prev == '+' && c == '/') { depth--; c = 0; }
        prev = c;
    }
}

static void saltar_espacios_y_comentarios(void) {
    int saltando_ruido = 1;

    while (saltando_ruido) {
        //char c = peek();
        char c = leer_char();

        if (isspace((unsigned char)c)) {
            continue;
        }

        if (c == '/') {
            char sig = leer_char();

            if (sig == '/') { saltar_comentario_linea();   }
            else if (sig == '*') { saltar_comentario_bloque();  }
            else if (sig == '+') { saltar_comentario_anidado(); }
            else {
                devolver_char();
                devolver_char();
                saltando_ruido = 0;
            }
        } else {
            saltando_ruido = 0;
        }
    }
}

static ComponenteLexico leer_identificador(void) {
    int longitud = 1;
    int col_inicio = columna_actual - 1;

    char c = leer_char();
    while (isalnum((unsigned char)c) || c == '_') {
        longitud++;
        c = leer_char();
    }

    if (longitud > MAX_LONGITUD_ID) {
        fprintf(stderr, "Aviso [%d:%d]: identificador supera el limite de %d caracteres\n", linea_actual, col_inicio, MAX_LONGITUD_ID);
    }

    char *lex = get_lexema();
    int token = buscar_o_insertar_TS(lex, IDENTIFICADOR);

    return make_cl(token, lex);
}


// Consume digitos hexadecimales con separador '_' 
static void consumir_hex(void) {
    char c = leer_char();
    while (isxdigit((unsigned char)c) || c == '_') c = leer_char();
}

// Consume digitos binarios con separador '_' 
static void consumir_bin(void) {
    char c = leer_char();
    while (c == '0' || c == '1' || c == '_') c = leer_char();
}

// Consume digitos decimales con separador '_' 
static void consumir_dec(void) {
    char c = leer_char();
    while (isdigit((unsigned char)c) || c == '_') c = leer_char();
}


/*static int es_binario(char c) {
    return c == '0' || c == '1';
}

static void consumir(int (*cond)(int)) {
    char c = leer_char();
    while (cond((unsigned char)c) || c == '_') c = leer_char();
}*/


// Intenta leer parte decimal (.digitos). Devuelve 1 si la leyo
static int leer_parte_decimal(void) {
    char c = leer_char();
    if (c != '.') return 0;

    if (isdigit((unsigned char)c)) {
        consumir_dec();
        return 1;
    }

    devolver_char();
    return 0;
}

// Intenta leer exponente (e/E [+-] digitos). Devuelve 1 si lo leyo
static int leer_exponente(void) {
    char c = leer_char();
    if (c != 'e' && c != 'E') return 0;

    char sig = leer_char();

    if (sig == '+' || sig == '-') {
        leer_char();
        sig = leer_char();
    }

    if (isdigit((unsigned char)sig)) {
        consumir_dec();
        return 1;
    }

    // No hay digitos tras 'e': backtrack
    devolver_char();
    char p = leer_char();
    if (p == '+' || p == '-') devolver_char();
    return 0;
}

static ComponenteLexico leer_numero(char primero) {
    int es_flotante = 0;

    if (primero == '0') {
        char c = leer_char();

        if (c == 'x' || c == 'X') {
            consumir_hex();
            //consumir(isxdigit);
            return make_cl(LIT_ENTERO, get_lexema());
        } else if (c == 'b' || c == 'B') {
            consumir_bin();
            //consumir(es_binario);
            return make_cl(LIT_ENTERO, get_lexema());
        }
    }

    //consumir(isdigit);
    consumir_dec();


    if (leer_parte_decimal()) es_flotante = 1;
    if (leer_exponente()) es_flotante = 1;

    if (es_flotante)
        return make_cl(LIT_FLOTANTE, get_lexema());

    return make_cl(LIT_ENTERO, get_lexema());
}


static ComponenteLexico leer_string(void) {
    char c = leer_char();

    while (c != '"' && c != (char)EOF) {
        if (c == '\\') {
            char esc = leer_char();
            if (esc != 'n' && esc != 't' && esc != 'r' && esc != '\\' && esc != '"') {
                fprintf(stderr, "Aviso [%d:%d]: secuencia de escape desconocida '\\%c'\n",
                        linea_actual, columna_actual, esc);
            }
        }
        c = leer_char();
    }

    return make_cl(LIT_STRING, get_lexema());
}


static ComponenteLexico procesar_operador_o_delimitador(char c) {
    switch (c) {
    case '/': return make_cl(match('=') ? OP_DIV_ASSIGN : '/',  get_lexema());
    case '=': return make_cl(match('=') ? OP_EQ  : '=',        get_lexema());
    case '!': return make_cl(match('=') ? OP_NEQ : '!',        get_lexema());
    case '&': return make_cl(match('&') ? OP_AND : '&',        get_lexema());
    case '|': return make_cl(match('|') ? OP_OR  : '|',        get_lexema());
    case '*': return make_cl(match('=') ? OP_MUL_ASSIGN : '*', get_lexema());

    case '<':
        if (match('=')) return make_cl(OP_LEQ, get_lexema());
        if (match('<')) return make_cl(OP_SHL, get_lexema());
        return make_cl('<', get_lexema());

    case '>':
        if (match('=')) return make_cl(OP_GEQ, get_lexema());
        if (match('>')) return make_cl(OP_SHR, get_lexema());
        return make_cl('>', get_lexema());

    case '+':
        if (match('+')) return make_cl(OP_INC,         get_lexema());
        if (match('=')) return make_cl(OP_PLUS_ASSIGN, get_lexema());
        return make_cl('+', get_lexema());

    case '-':
        if (match('-')) return make_cl(OP_DEC,          get_lexema());
        if (match('=')) return make_cl(OP_MINUS_ASSIGN, get_lexema());
        return make_cl('-', get_lexema());

    case '%':
        if (match('=')) return make_cl(OP_MOD_ASSIGN, get_lexema());
        return make_cl('%', get_lexema());

    case ';': case ',': case '.':
    case '(': case ')': case '[': case ']':
    case '{': case '}': case '~': case '^':
        return make_cl(c, get_lexema());

    default:
        error_lexico(c);
        return make_cl(TOKEN_ERROR, get_lexema());
    }
}


ComponenteLexico sig_comp_lexico(void) {
    saltar_espacios_y_comentarios();

    mover_inicio();
    char c = leer_char();

    if (c == (char)EOF)
        return make_cl(TOKEN_EOF, get_lexema());

    if (isalpha((unsigned char)c) || c == '_')
        return leer_identificador();

    if (isdigit((unsigned char)c))
        return leer_numero(c);

    if (c == '"')
        return leer_string();

    return procesar_operador_o_delimitador(c);
}
