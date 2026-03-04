/*
 * tabla_simbolos.c  (TS.c)
 *
 * Tabla de simbolos implementada como tabla hash con
 * direccionamiento abierto (sondeo lineal).
 *
 * Estructura interna:
 *   - Array estatico de TAM_HASH entradas (definido en definiciones.h).
 *   - Funcion hash FNV-1a para distribucion uniforme.
 *   - Colisiones resueltas por sondeo lineal: slot = (slot+1) % TAM_HASH.
 *   - Cada entrada almacena: lexema (dinamico), componente, linea, columna.
 *
 * API publica:
 *   - buscarSimbolo(lexema)   -> indice del slot, o -1 si no existe.
 *   - insertarSimbolo(...)    -> indice del slot (nuevo o existente).
 *   - obtenerComponente(idx) -> codigo del token en ese slot.
 *   - obtenerLexema(idx)     -> texto del lexema en ese slot.
 *
 * Las funciones de impresion recorren todos los slots ocupados.
 * No se necesita realloc: el tamano es estatico y suficiente para
 * los simbolos del lenguaje D en regression.d.
 */

#include "definiciones.h"

/* ============================================================
 * Estructura de una entrada en la tabla hash
 * ============================================================ */
typedef struct {
    char *lexema;    /* Cadena dinamica (malloc/strdup) */
    int   componente;
    int   linea;
    int   columna;
    int   ocupado;   /* 0 = slot libre, 1 = slot ocupado */
} EntradaHash;

/* Array estatico de TAM_HASH slots */
static EntradaHash tabla[TAM_HASH];
static int cantidad = 0; /* Entradas actualmente ocupadas */

/* ============================================================
 * hashear: funcion hash FNV-1a sobre la cadena lexema.
 * Produce valores en [0, TAM_HASH).
 * ============================================================ */
static unsigned int hashear(const char *lexema) {
    unsigned int h = 2166136261u; /* offset FNV */
    while (*lexema) {
        h ^= (unsigned char)(*lexema++);
        h *= 16777619u; /* primo FNV */
    }
    return h % (unsigned int)TAM_HASH;
}

/* ============================================================
 * inicializarTablaSimbolos: pone todos los slots a libre y
 * pre-carga las palabras reservadas del lenguaje D.
 * ============================================================ */
void inicializarTablaSimbolos(void) {
    memset(tabla, 0, sizeof(tabla));
    cantidad = 0;

    /* Pre-cargar palabras reservadas */
    insertarSimbolo("import",   IMPORT,   0, 0);
    insertarSimbolo("while",    WHILE,    0, 0);
    insertarSimbolo("double",   DOUBLE,   0, 0);
    insertarSimbolo("int",      INT,      0, 0);
    insertarSimbolo("void",     VOID,     0, 0);
    insertarSimbolo("foreach",  FOREACH,  0, 0);
    insertarSimbolo("cast",     CAST,     0, 0);
    insertarSimbolo("return",   RETURN,   0, 0);
    insertarSimbolo("enforce",  ENFORCE,  0, 0);
    insertarSimbolo("if",       IF,       0, 0);
    insertarSimbolo("else",     ELSE,     0, 0);
    insertarSimbolo("for",      FOR,      0, 0);
    insertarSimbolo("do",       DO,       0, 0);
    insertarSimbolo("break",    BREAK,    0, 0);
    insertarSimbolo("continue", CONTINUE, 0, 0);
    insertarSimbolo("switch",   SWITCH,   0, 0);
    insertarSimbolo("case",     CASE,     0, 0);
    insertarSimbolo("default",  DEFAULT,  0, 0);

    imprimirPalabrasReservadas();
}

/* ============================================================
 * liberarTablaSimbolos: libera los lexemas dinamicos y limpia
 * todos los slots. Deja los punteros a NULL (sin punteros colgantes).
 * ============================================================ */
void liberarTablaSimbolos(void) {
    for (int i = 0; i < TAM_HASH; i++) {
        if (tabla[i].ocupado && tabla[i].lexema != NULL) {
            free(tabla[i].lexema);
            tabla[i].lexema = NULL;
        }
        tabla[i].ocupado = 0;
    }
    cantidad = 0;
}

/* ============================================================
 * buscarSimbolo: busca lexema en la tabla hash.
 *
 * Retorna el indice del slot si se encuentra, -1 si no existe.
 * Usa sondeo lineal para resolver colisiones.
 * ============================================================ */
int buscarSimbolo(const char *lexema) {
    unsigned int slot = hashear(lexema);
    int intentos = 0;

    while (intentos < TAM_HASH) {
        if (!tabla[slot].ocupado) {
            /* Slot libre: el lexema no esta en la tabla */
            return -1;
        }
        if (strcmp(tabla[slot].lexema, lexema) == 0) {
            return (int)slot;
        }
        slot = (slot + 1) % TAM_HASH;
        intentos++;
    }
    return -1;
}

/* ============================================================
 * insertarSimbolo: inserta un lexema en la tabla hash.
 *
 * Si ya existe, devuelve su indice sin duplicar.
 * Si la tabla esta llena (> 75% de carga), reporta error.
 *
 * Retorna el indice del slot asignado, o -1 en caso de error.
 * ============================================================ */
int insertarSimbolo(const char *lexema, int componente, int linea, int columna) {
    /* Evitar duplicados */
    int existente = buscarSimbolo(lexema);
    if (existente != -1) {
        return existente;
    }

    /* Comprobar factor de carga (maximo 75%) */
    if (cantidad >= TAM_HASH * 3 / 4) {
        fprintf(stderr, "Error: Tabla de simbolos llena (factor de carga superado)\n");
        return -1;
    }

    /* Buscar el primer slot libre por sondeo lineal */
    unsigned int slot = hashear(lexema);
    while (tabla[slot].ocupado) {
        slot = (slot + 1) % TAM_HASH;
    }

    tabla[slot].lexema     = strdup(lexema);
    tabla[slot].componente = componente;
    tabla[slot].linea      = linea;
    tabla[slot].columna    = columna;
    tabla[slot].ocupado    = 1;
    cantidad++;

    return (int)slot;
}

/* ============================================================
 * obtenerComponente: devuelve el codigo del token en el slot dado.
 * ============================================================ */
int obtenerComponente(int indice) {
    if (indice >= 0 && indice < TAM_HASH && tabla[indice].ocupado) {
        return tabla[indice].componente;
    }
    return ERROR_LEXICO;
}

/* ============================================================
 * obtenerLexema: devuelve el texto del lexema en el slot dado.
 * ============================================================ */
const char *obtenerLexema(int indice) {
    if (indice >= 0 && indice < TAM_HASH && tabla[indice].ocupado) {
        return tabla[indice].lexema;
    }
    return "";
}

/* ============================================================
 * imprimirTablaSimbolos: imprime todas las entradas ocupadas.
 * ============================================================ */
void imprimirTablaSimbolos(void) {
    printf("\n========== TABLA DE SIMBOLOS (HASH) ==========\n");
    printf("%-40s %-15s %s\n", "Lexema", "Componente", "Linea:Columna");
    printf("===============================================\n");

    int impresos = 0;
    for (int i = 0; i < TAM_HASH; i++) {
        if (tabla[i].ocupado) {
            printf("%-40s %-15d %d:%d\n",
                   tabla[i].lexema,
                   tabla[i].componente,
                   tabla[i].linea,
                   tabla[i].columna);
            impresos++;
        }
    }

    printf("===============================================\n");
    printf("Total de entradas: %d  (slots usados / TAM_HASH = %d / %d)\n",
           impresos, impresos, TAM_HASH);
}

/* ============================================================
 * imprimirPalabrasReservadas: imprime solo las keywords.
 * ============================================================ */
void imprimirPalabrasReservadas(void) {
    printf("========== PALABRAS RESERVADAS ==========\n");
    for (int i = 0; i < TAM_HASH; i++) {
        if (tabla[i].ocupado &&
            tabla[i].componente >= 273 &&
            tabla[i].componente < 300) {
            printf("  %-20s (codigo: %d)\n",
                   tabla[i].lexema,
                   tabla[i].componente);
        }
    }
    printf("=========================================\n\n");
}

/* ============================================================
 * imprimirIdentificadores: imprime solo los identificadores
 * encontrados durante el analisis (no las palabras reservadas).
 * ============================================================ */
void imprimirIdentificadores(void) {
    printf("\n========== IDENTIFICADORES ==========\n");
    for (int i = 0; i < TAM_HASH; i++) {
        if (tabla[i].ocupado && tabla[i].componente == IDENTIFICADOR) {
            printf("  %-30s (linea: %d, columna: %d)\n",
                   tabla[i].lexema,
                   tabla[i].linea,
                   tabla[i].columna);
        }
    }
    printf("=====================================\n");
}
