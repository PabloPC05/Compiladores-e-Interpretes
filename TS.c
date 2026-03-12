#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TS.h"
#include "definiciones.h"
#include "errores.h"

#define TAM_TS 101

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

static ComponenteLexico *tabla = NULL;


void inicializar_TS(void) {
    tabla = malloc(TAM_TS * sizeof(ComponenteLexico));
    if (!tabla) {
        report(ERR_MEMORIA_INSUFICIENTE, 0, 0, 1);
    }

    for (int i = 0; i < TAM_TS; i++) {
        tabla[i].lexema = NULL;
        tabla[i].token  = TOKEN_INVALIDO;
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

    printf("=== Palabras reservadas cargadas en la tabla de simbolos ===\n");
    imprimir_TS();
    printf("============================================================\n\n");
}


static inline int buscar_posicion(const char *lexema, unsigned int h) {
    unsigned int pos = h % TAM_TS;

    for (int i = 0; i < TAM_TS; i++) {
        unsigned int p = (pos + i) % TAM_TS;

        if (tabla[p].lexema == NULL)
            return p;

        if (strcmp(tabla[p].lexema, lexema) == 0)
            return p;
    }

    return -1; // tabla llena
}


ComponenteLexico buscar_o_insertar_TS(const char *lexema, int token_nuevo) {
    unsigned int h = hash_djb2(lexema);
    int pos = buscar_posicion(lexema, h);

    // Si la tabla está llena, no se puede insertar ni buscar
    if (pos == -1) {
        report(ERR_TS_LLENA, 0, 0, 0);
        return make_cl(TOKEN_INVALIDO, NULL);
    }

    // Si el lexema ya existe en la tabla, devolvemos su lexema
    if (tabla[pos].lexema != NULL)
        return tabla[pos];

    // Si el lexema no existe, lo insertamos con el token dado y devolvemos el nuevo componente léxico
    tabla[pos].lexema = copiar_lexema(lexema); // Diferente puntero aunque el contenido sea el mismo
    if (!tabla[pos].lexema) {
        report(ERR_MEMORIA_INSUFICIENTE, 0, 0, 1);
    }
    
    tabla[pos].token = token_nuevo;
    return tabla[pos];
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
