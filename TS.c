#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TS.h"
#include "definiciones.h"
#include "errores.h"

#define TAM_TS_INICIAL  101
#define FACTOR_CARGA_MAX 70  

static inline char *copiar_lexema(const char *src) {
    char *copia = malloc(strlen(src) + 1);
    if (copia) strcpy(copia, src);
    return copia;
}

/* Hash djb2: h = h*33 + c. Devuelve el hash crudo (sin modulo) */
static inline unsigned int hash_djb2(const char *str) {
    unsigned long h = 5381;
    while (*str)
        h = h * 33 + (unsigned char)*str++;
    return (unsigned int)h;
}

typedef struct {
    ComponenteLexico *tabla;
    int tam_actual;
    int num_entradas;
} TablaSimbolos;

static TablaSimbolos ts = { .tabla = NULL, .tam_actual = 0, .num_entradas = 0 };


/* Redimensiona la tabla al doble de su tamaño actual */
static void redimensionar_TS(void) {
    int nuevo_tam = ts.tam_actual * 2 + 1;

    /* Reservar nueva tabla e inicializarla */
    ComponenteLexico *nueva = malloc(nuevo_tam * sizeof(ComponenteLexico));
    if (!nueva) report(ERR_MEMORIA_INSUFICIENTE, 0, 0, 1);

    for (int i = 0; i < nuevo_tam; i++) {
        nueva[i].lexema = NULL;
        nueva[i].token  = TOKEN_INVALIDO;
    }

    /* Reinsertar cada entrada en la nueva tabla recalculando posiciones */
    for (int i = 0; i < ts.tam_actual; i++) {
        if (ts.tabla[i].lexema == NULL) continue;

        unsigned int pos = hash_djb2(ts.tabla[i].lexema) % nuevo_tam;
        while (nueva[pos].lexema != NULL)
            pos = (pos + 1) % nuevo_tam;

        /* Mover el puntero al lexema, no copiarlo */
        nueva[pos].lexema = ts.tabla[i].lexema;
        nueva[pos].token  = ts.tabla[i].token;
    }

    /* Liberar el array viejo (no los lexemas) y actualizar */
    free(ts.tabla);
    ts.tabla = nueva;
    ts.tam_actual = nuevo_tam;
}


void inicializar_TS(void) {
    ts.tam_actual = TAM_TS_INICIAL;
    ts.num_entradas = 0;

    ts.tabla = malloc(ts.tam_actual * sizeof(ComponenteLexico));
    if (!ts.tabla) {
        report(ERR_MEMORIA_INSUFICIENTE, 0, 0, 1);
    }

    for (int i = 0; i < ts.tam_actual; i++) {
        ts.tabla[i].lexema = NULL;
        ts.tabla[i].token  = TOKEN_INVALIDO;
    }

    /* Palabras reservadas presentes en regression.d */
    buscar_o_insertar_TS("import",   KW_IMPORT);
    buscar_o_insertar_TS("while",    KW_WHILE);
    buscar_o_insertar_TS("double",   KW_DOUBLE);
    buscar_o_insertar_TS("int",      KW_INT);
    buscar_o_insertar_TS("void",     KW_VOID);
    buscar_o_insertar_TS("foreach",  KW_FOREACH);
    buscar_o_insertar_TS("cast",     KW_CAST);
    buscar_o_insertar_TS("enforce",  KW_ENFORCE);
    buscar_o_insertar_TS("return",   KW_RETURN);
    buscar_o_insertar_TS("if",       KW_IF);
    buscar_o_insertar_TS("else",     KW_ELSE);
    buscar_o_insertar_TS("for",      KW_FOR);
    buscar_o_insertar_TS("do",       KW_DO);
    buscar_o_insertar_TS("break",    KW_BREAK);
    buscar_o_insertar_TS("continue", KW_CONTINUE);
    buscar_o_insertar_TS("switch",   KW_SWITCH);
    buscar_o_insertar_TS("case",     KW_CASE);
    buscar_o_insertar_TS("default",  KW_DEFAULT);
}


static inline int buscar_posicion(const char *lexema, unsigned int h) {
    unsigned int pos = h % ts.tam_actual;

    for (int i = 0; i < ts.tam_actual; i++) {
        unsigned int p = (pos + i) % ts.tam_actual;

        if (ts.tabla[p].lexema == NULL)
            return p;

        if (strcmp(ts.tabla[p].lexema, lexema) == 0)
            return p;
    }

    return -1; // tabla llena
}


ComponenteLexico buscar_o_insertar_TS(const char *lexema, int token_nuevo) {
    /* Si el factor de carga supera el 70%, redimensionar antes de insertar */
    if (ts.num_entradas * 100 / ts.tam_actual > FACTOR_CARGA_MAX)
        redimensionar_TS();

    unsigned int h = hash_djb2(lexema);
    int pos = buscar_posicion(lexema, h);

    // Si la tabla está llena (no debería ocurrir tras redimensionar)
    if (pos == -1) {
        report(ERR_TS_LLENA, 0, 0, 1);
        return make_cl(TOKEN_INVALIDO, NULL);
    }

    // Si el lexema ya existe en la tabla, devolvemos su componente léxico
    if (ts.tabla[pos].lexema != NULL)
        return ts.tabla[pos];

    // Si el lexema no existe, lo insertamos con el token dado
    ts.tabla[pos].lexema = copiar_lexema(lexema);
    if (!ts.tabla[pos].lexema) {
        report(ERR_MEMORIA_INSUFICIENTE, 0, 0, 1);
    }
    ts.tabla[pos].token = token_nuevo;
    ts.num_entradas++;

    return ts.tabla[pos];
}


void imprimir_TS(void) {
    printf("=== Tabla de simbolos ===\n");
    for (int i = 0; i < ts.tam_actual; i++) {
        if (ts.tabla[i].lexema != NULL)
            printf("  [%3d] token=%-4d  lexema=%s\n", i, ts.tabla[i].token, ts.tabla[i].lexema);
    }
    printf("=========================\n\n");
}


void liberar_TS(void) {
    for (int i = 0; i < ts.tam_actual; i++) {
        if (ts.tabla[i].lexema != NULL) {
            free(ts.tabla[i].lexema);
            ts.tabla[i].lexema = NULL;
        }
    }
    free(ts.tabla);
    ts.tabla = NULL;
    ts.tam_actual = 0;
    ts.num_entradas = 0;
}
