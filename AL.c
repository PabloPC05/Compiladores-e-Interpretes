#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "AL.h"
#include "SI.h"
#include "TS.h"
#include "errores.h"
#include "definiciones.h"

// Verifica si el caracter actual coincide con el esperado
static inline int match(int esperado) {
    int c = sig_caracter();
    if (c == esperado) return 1;
    if (c != EOF) devolver_caracter();
    return 0;
}

// Comentario de linea: // ... \n
static void saltar_comentario_linea(void) {
    int c;
    do {
        c = sig_caracter();
    } while (c != '\n' && c != EOF);
}

// Comentario de bloque /* ... */
static void saltar_comentario_bloque(void) {
    int c;
    do {
        c = sig_caracter();
        // Consumimos caracteres hasta encontrar la secuencia de cierre "*/". Si encontramos EOF antes, reportamos error
    } while (c != EOF && !(c == '*' && match('/')));
    if (c == EOF)
        report(obtener_linea(), obtener_columna(), ERR_EOF_COMENTARIO_BLQ);
}

// Comentario anidado: /+ ... +/ 
static void saltar_comentario_anidado(void) {
    int depth = 1; // Profundidad de anidamiento
    int c; // Caracter actual
    while (depth > 0) { // Mientras haya comentarios abiertos
        c = sig_caracter(); // Leemos el siguiente caracter
        if (c == EOF) { // Si llegamos al final del archivo antes de cerrar todos los comentarios, reportamos error
            report(obtener_linea(), obtener_columna(), ERR_EOF_COMENTARIO_ANID);
            return;
        }
        // Si encontramos el inicio de un nuevo comentario anidado, incrementamos la profundidad
        if (c == '/' && match('+'))      depth++;
        // Si encontramos el final de un comentario, decrementamos la profundidad
        else if (c == '+' && match('/')) depth--;
    }
}

static void saltar_espacios_y_comentarios(void) {
    int saltando_ruido = 1; 

    // Mientras sigamos encontrando espacios o comentarios, seguimos saltando
    while (saltando_ruido) {
        // Actualizamos el bloque de inicio al bloque activo y colocamos el puntero de inicio en la posición del puntero de delantero para que el lexema que se forme a continuación sea correcto incluso si hemos tenido que cambiar de bloque
        mover_inicio();
        int c = sig_caracter();

        // Si el caracter es un espacio seguimos saltando
        if (isspace(c)) continue;

        // Si estamos ante un posible comentario
        if (c == '/') {
            // Pedimos el siguiente caracter
            int sig = sig_caracter();

            // Determinamos el tipo de caracter posible 
            if (sig == '/')      { saltar_comentario_linea();   }
            else if (sig == '*') { saltar_comentario_bloque();  }
            else if (sig == '+') { saltar_comentario_anidado(); }

            // Si no es ninguno de los tipos de comentarios posibles
            else {
                // Si el siguiente caracter no es EOF, lo devolvemos para que pueda ser procesado como parte de otro token
                if (sig != EOF) devolver_caracter();
                devolver_caracter();

                // No estamos ante un comentario, por lo que dejamos de saltar
                saltando_ruido = 0;
            }
        } else {
            // Si tampoco había espacios que saltar ni comentarios, devolvemos el caracter para que pueda ser procesado como parte de otro token y dejamos de saltar
            if (c != EOF) devolver_caracter();
            saltando_ruido = 0;
        }
    }
}

// Lee un identificador o palabra reservada
static ComponenteLexico leer_identificador(void) {
    int longitud = 1;
    int col_inicio = obtener_columna();

    int c = sig_caracter();
    while (isalnum(c) || c == '_') {
        longitud++;
        c = sig_caracter();
    }
    // Si el caracter que ha detenido la lectura no es EOF, lo devolvemos para que pueda ser procesado como parte de otro token
    if (c != EOF) devolver_caracter();

    // Si el identificador supera el límite de longitud, reportamos error pero seguimos procesándolo para no perder más errores posibles 
    if (longitud > TAM_MAX_LEXEMA) {
        report(obtener_linea(), col_inicio, ERR_ID_DEMASIADO_LARGO);
    }

    // Buscamos el lexema en la tabla de símbolos. Si no está, se insertará con token IDENTIFICADOR. Devolvemos el componente léxico resultante
    char *lex = get_lexema();
    ComponenteLexico cl = buscar_o_insertar_TS(lex, IDENTIFICADOR);
    return make_cl(cl.token, lex);
}


// Consume digitos hexadecimales con separador '_'. Devuelve num de digitos reales.
static inline int consumir_hex(void) {
    int n = 0;
    int c = sig_caracter();
    while (isxdigit(c) || c == '_') {
        if (isxdigit(c)) n++;
        c = sig_caracter();
    }
    if (c != EOF) devolver_caracter();
    return n;
}

// Consume digitos binarios con separador '_'. Devuelve num de digitos reales.
static inline int consumir_bin(void) {
    int n = 0;
    int c = sig_caracter();
    while (c == '0' || c == '1' || c == '_') {
        if (c != '_') n++;
        c = sig_caracter();
    }
    if (c != EOF) devolver_caracter();
    return n;
}

// Consume digitos decimales con separador '_' 
static inline void consumir_dec(void) {
    int c = sig_caracter();
    while (isdigit(c) || c == '_') c = sig_caracter();
    if (c != EOF) devolver_caracter();
}


// Intenta leer parte decimal (.digitos). Devuelve 1 si la leyo
static inline int leer_parte_decimal(void) {
    int c = sig_caracter();
    // Si no es un punto, devolvemos el caracter para que pueda ser procesado como parte de otro token
    if (c != '.') {
        if (c != EOF) devolver_caracter();
        return 0;
    }

    // Si tenemos un punto, el siguiente caracter debe ser un digito para que sea parte decimal
    c = sig_caracter();
    if (isdigit(c)) {
        consumir_dec();
        return 1;
    }

    // Si el siguiente caracter no es un dígito, debemos devolver el caracter leído para que pueda ser procesado como parte de otro token
    if (c != EOF) devolver_caracter();
    devolver_caracter();
    return 0;
}

// Intenta leer exponente (e/E [+-] digitos). Devuelve 1 si lo leyo
static inline int leer_exponente(void) {
    // Tomamos el siguiente caracter para ver si es 'e' o 'E'
    int c = sig_caracter();
    if (c != 'e' && c != 'E') {
        // Si no lo es y no es EOF, lo devolvemos para que pueda ser procesado como parte de otro token
        if (c != EOF) devolver_caracter();
        return 0;
    }

    // Si tenemos 'e' o 'E', el siguiente caracter puede ser un signo opcional
    int sig = sig_caracter();
    // Si es un signo 
    if (sig == '+' || sig == '-') {
        // Si tras el signo hay digitos, consumimos el exponente y devolvemos 1
        int siguiente = sig_caracter();
        if (isdigit(siguiente)) {
            consumir_dec();
            return 1;
        }
        // Si no hay digitos tras el signo debemos volver a atrás para que pueda ser procesado como parte de otro token
        if (siguiente != EOF) devolver_caracter();
        devolver_caracter();   // retrocedemos el signo
        devolver_caracter();   // retrocedemos la 'e' o 'E'
        return 0;
    }

    // Si tras la 'e' o 'E' hay digitos, consumimos el exponente y devolvemos 1
    if (isdigit(sig)) {
        consumir_dec();
        return 1;
    }

    // Si no hay digitos tras la 'e' o 'E', debemos devoler el caracter leído para que pueda ser procesado como parte de otro token
    if (sig != EOF) devolver_caracter();
    devolver_caracter();   // retrocedemos la 'e' o 'E'
    return 0;
}

static ComponenteLexico leer_numero(int primero) {
    int es_flotante = 0;

    if (primero == '0') {
        int c = sig_caracter();

        if (c == 'x' || c == 'X') {

            // Si no han consumido dígitos hexadecimales tras el prefijo, reportamos error
            if (consumir_hex() == 0) {
                report(obtener_linea(), obtener_columna(), ERR_HEX_SIN_DIGITOS);
                return make_cl(TOKEN_ERROR, get_lexema());
            }
            return make_cl(LIT_ENTERO, get_lexema());
            
        } else if (c == 'b' || c == 'B') {

            // Si no se ha consumido ningun digito binario tras el prefijo, reportamos error
            if (consumir_bin() == 0) {
                report(obtener_linea(), obtener_columna(), ERR_BIN_SIN_DIGITOS);
                return make_cl(TOKEN_ERROR, get_lexema());
            }
            return make_cl(LIT_ENTERO, get_lexema());
        }

        if (c != EOF) devolver_caracter();
    }

    // Decimal o flotante: devolver primero para que consumir_dec lo relea
    devolver_caracter();
    consumir_dec();

    if (leer_parte_decimal()) es_flotante = 1;
    if (leer_exponente()) es_flotante = 1;

    if (es_flotante)
        return make_cl(LIT_FLOTANTE, get_lexema());

    return make_cl(LIT_ENTERO, get_lexema());
}

// Autómata para leer cadenas de caracteres (strings)
static ComponenteLexico leer_string(void) {
    int c = sig_caracter();

    while (c != '"' && c != EOF) {
        if (c == '\\') {
            int esc = sig_caracter();
            if (esc != 'n' && esc != 't' && esc != 'r' && esc != '\\' && esc != '"') {
                report(obtener_linea(), obtener_columna(), ERR_ESCAPE_DESCONOCIDO);
            }
        }
        c = sig_caracter();
    }

    if (c == EOF) {
        report(obtener_linea(), obtener_columna(), ERR_STR_SIN_CERRAR);
        return make_cl(TOKEN_ERROR, get_lexema());
    }

    return make_cl(LIT_STRING, get_lexema());
}


static ComponenteLexico procesar_operador_o_delimitador(int c) {
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
        report(obtener_linea(), obtener_columna(), ERR_CARACTER_NO_RECONOC);
        return make_cl(TOKEN_ERROR, get_lexema());
    }
}


ComponenteLexico sig_comp_lexico(void) {
    saltar_espacios_y_comentarios();

    mover_inicio();
    int c = sig_caracter();

    if (c == EOF)
        return make_cl(TOKEN_EOF, get_lexema());

    if (isalpha(c) || c == '_')
        return leer_identificador();

    if (isdigit(c))
        return leer_numero(c);

    if (c == '"')
        return leer_string();

    return procesar_operador_o_delimitador(c);
}

void liberar_comp_lexico(ComponenteLexico *cl) {
    free(cl->lexema);
    cl->lexema = NULL;
}
