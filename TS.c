#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TS.h"
#include "definiciones.h"

#define TAM_TS 101

static char *copiar_lexema(const char *src) {
    char *copia = malloc(strlen(src) + 1);
    if (copia) strcpy(copia, src);
    return copia;
}

/* Hash djb2: h = h*33 + c. Devuelve el hash crudo (sin modulo) */
static unsigned int hash_djb2(const char *str) {
    unsigned long h = 5381;
    while (*str)
        h = h * 33 + (unsigned char)*str++;
    return (unsigned int)h;
}

typedef struct {
    char         *lexema;
    unsigned int  hash;   // hash crudo, cache para evitar strcmp innecesarios
    int           token;
} EntradaTS;

static EntradaTS *tabla = NULL;


void inicializar_TS(void) {
    tabla = malloc(TAM_TS * sizeof(EntradaTS));
    if (!tabla) {
        fprintf(stderr, "Error: memoria insuficiente para la tabla de simbolos\n");
        exit(1);
    }

    for (int i = 0; i < TAM_TS; i++) {
        tabla[i].lexema = NULL;
        tabla[i].hash   = 0;
        tabla[i].token  = TOKEN_INVALIDO;
    }

    /* Palabras reservadas presentes en regression.d */
    insertar_TS("import",   KW_IMPORT);
    insertar_TS("while",    KW_WHILE);
    insertar_TS("double",   KW_DOUBLE);
    insertar_TS("int",      KW_INT);
    insertar_TS("void",     KW_VOID);
    insertar_TS("foreach",  KW_FOREACH);
    insertar_TS("cast",     KW_CAST);
    insertar_TS("enforce",  KW_ENFORCE);
    insertar_TS("return",   KW_RETURN);
    insertar_TS("if",       KW_IF);
    insertar_TS("else",     KW_ELSE);
    insertar_TS("for",      KW_FOR);
    insertar_TS("do",       KW_DO);
    insertar_TS("break",    KW_BREAK);
    insertar_TS("continue", KW_CONTINUE);
    insertar_TS("switch",   KW_SWITCH);
    insertar_TS("case",     KW_CASE);
    insertar_TS("default",  KW_DEFAULT);

    printf("=== Palabras reservadas cargadas en la tabla de simbolos ===\n");
    imprimir_TS();
    printf("============================================================\n\n");
}


/*
 * Busca la posicion de un lexema en la tabla.
 * Recibe el hash crudo (sin modulo) para compararlo con la cache de cada
 * entrada antes de recurrir a strcmp. En una tabla pequena (101 posiciones)
 * las colisiones son frecuentes, y comparar dos enteros es mucho mas rapido
 * que comparar cadenas caracter a caracter. Solo cuando los hashes coinciden
 * se confirma con strcmp para descartar colisiones de hash.
 */
static int buscar_posicion(const char *lexema, unsigned int h) {
    unsigned int pos = h % TAM_TS;

    for (int i = 0; i < TAM_TS; i++) {
        unsigned int p = (pos + i) % TAM_TS;

        if (tabla[p].lexema == NULL)
            return p;

        if (tabla[p].hash == h && strcmp(tabla[p].lexema, lexema) == 0)
            return p;
    }

    return -1; // tabla llena
}

/* int buscar_TS(const char *lexema) {
    unsigned int h = hash_djb2(lexema);
    int pos = buscar_posicion(lexema, h);
    if (pos == -1 || tabla[pos].lexema == NULL)
        return TOKEN_INVALIDO;
    return tabla[pos].token;
} */


void insertar_TS(const char *lexema, int token) {
    unsigned int h = hash_djb2(lexema);
    int pos = buscar_posicion(lexema, h);
    if (pos == -1) {
        fprintf(stderr, "Error: tabla de simbolos llena, no se puede insertar '%s'\n", lexema);
        return;
    }

    if (tabla[pos].lexema != NULL) {
        return;
    }

    tabla[pos].lexema = copiar_lexema(lexema);
    if (!tabla[pos].lexema) {
        fprintf(stderr, "Error: memoria insuficiente en insertar_TS\n");
        exit(1);
    }
    tabla[pos].hash  = h;
    tabla[pos].token = token;

}


int buscar_o_insertar_TS(const char *lexema, int token_nuevo) {
    unsigned int h = hash_djb2(lexema);
    int pos = buscar_posicion(lexema, h);

    if (pos == -1) {
        fprintf(stderr, "Error: tabla de simbolos llena, no se puede insertar '%s'\n", lexema);
        return token_nuevo;
    }

    if (tabla[pos].lexema != NULL)
        return tabla[pos].token;

    tabla[pos].lexema = copiar_lexema(lexema);
    if (!tabla[pos].lexema) {
        fprintf(stderr, "Error: memoria insuficiente en buscar_o_insertar_TS\n");
        exit(1);
    }
    tabla[pos].hash  = h;
    tabla[pos].token = token_nuevo;
    return token_nuevo;
}


void imprimir_TS(void) {
    printf("=== Tabla de simbolos ===\n");
    for (int i = 0; i < TAM_TS; i++) {
        if (tabla[i].lexema != NULL)
            printf("  [%3d] token=%-4d  lexema=%s\n", i, tabla[i].token, tabla[i].lexema);
    }
    printf("=========================\n\n");
}


void liberar_TS(void) {
    for (int i = 0; i < TAM_TS; i++) {
        if (tabla[i].lexema != NULL) {
            free(tabla[i].lexema);
            tabla[i].lexema = NULL;
        }
    }
    free(tabla);
    tabla = NULL;
}
