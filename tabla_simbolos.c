/*
 * tabla_simbolos.c  (TS.c)
 *
 * Wrapper sobre tabla_hash_recolocacion para la tabla de simbolos
 * del analizador lexico.
 *
 * Responsabilidades:
 *   - Pre-cargar las palabras reservadas del lenguaje D al inicializar.
 *   - Exponer la API limpia usada por analizador.c:
 *       buscarSimbolo / insertarSimbolo / obtenerComponente / obtenerLexema
 *   - Imprimir la tabla al finalizar el analisis.
 *
 * Parametros hash fijados internamente (no expuestos al exterior):
 *   - tipoFH = 3  (polinomio de Horner, mejor distribucion)
 *   - K      = 31 (primo clasico para identificadores)
 *   - tipoR  = 1  (sondeo lineal, optimo con factor de carga < 0.5)
 *   - a      = 1  (paso unitario)
 */

#include "definiciones.h"
#include "tabla_hash_recolocacion.h"

/* Parametros hash fijos */
#define TIPO_FH   3
#define K_HASH    31
#define TIPO_R    1
#define A_SONDEO  1

/* Tabla hash interna (unica instancia) */
static TablaHash tabla;

/* ============================================================
 * inicializarTablaSimbolos
 * ============================================================ */
void inicializarTablaSimbolos(void) {
    InicializarTablaHash(tabla);

    insertarSimbolo("import",   IMPORT);
    insertarSimbolo("while",    WHILE);
    insertarSimbolo("double",   DOUBLE);
    insertarSimbolo("int",      INT);
    insertarSimbolo("void",     VOID);
    insertarSimbolo("foreach",  FOREACH);
    insertarSimbolo("cast",     CAST);
    insertarSimbolo("return",   RETURN);
    insertarSimbolo("enforce",  ENFORCE);
    insertarSimbolo("if",       IF);
    insertarSimbolo("else",     ELSE);
    insertarSimbolo("for",      FOR);
    insertarSimbolo("do",       DO);
    insertarSimbolo("break",    BREAK);
    insertarSimbolo("continue", CONTINUE);
    insertarSimbolo("switch",   SWITCH);
    insertarSimbolo("case",     CASE);
    insertarSimbolo("default",  DEFAULT);

    imprimirPalabrasReservadas();
}

/* ============================================================
 * liberarTablaSimbolos
 * Sin memoria dinamica que liberar: lexema es array estatico
 * dentro de TIPOELEMENTO. Solo se resetea la tabla.
 * ============================================================ */
void liberarTablaSimbolos(void) {
    InicializarTablaHash(tabla);
}

/* ============================================================
 * buscarSimbolo
 * Retorna el indice del slot si existe, -1 si no.
 * ============================================================ */
int buscarSimbolo(const char *lexema) {
    return BuscarHashSlot(tabla, (char *)lexema,
                          TIPO_FH, K_HASH, TIPO_R, A_SONDEO);
}

/* ============================================================
 * insertarSimbolo
 * Inserta el lexema con su componente. Si ya existe, no duplica.
 * Retorna el indice del slot (nuevo o existente), -1 si error.
 * ============================================================ */
int insertarSimbolo(const char *lexema, int componente) {
    /* Comprobar si ya existe */
    int slot = buscarSimbolo(lexema);
    if (slot != -1)
        return slot;

    /* Construir el elemento e insertarlo */
    TIPOELEMENTO e;
    strncpy(e.lexema, lexema, MAX_LONGITUD_ID - 1);
    e.lexema[MAX_LONGITUD_ID - 1] = '\0';
    e.componente = componente;

    int pasos = 0;
    InsertarHash(tabla, e, TIPO_FH, K_HASH, TIPO_R, A_SONDEO, &pasos);

    /* Devolver el slot donde quedo insertado */
    return buscarSimbolo(lexema);
}

/* ============================================================
 * obtenerComponente
 * ============================================================ */
int obtenerComponente(int indice) {
    return ObtenerComponenteHash(tabla, indice);
}

/* ============================================================
 * obtenerLexema
 * ============================================================ */
const char *obtenerLexema(int indice) {
    if (indice >= 0 && indice < N &&
        tabla[indice].lexema[0] != VACIO &&
        tabla[indice].lexema[0] != BORRADO) {
        return tabla[indice].lexema;
    }
    return "";
}

/* ============================================================
 * imprimirTablaSimbolos
 * ============================================================ */
void imprimirTablaSimbolos(void) {
    printf("\n========== TABLA DE SIMBOLOS ==========\n");
    printf("%-40s %s\n", "Lexema", "Componente");
    printf("========================================\n");

    int total = 0;
    for (int i = 0; i < N; i++) {
        if (tabla[i].lexema[0] != VACIO &&
            tabla[i].lexema[0] != BORRADO) {
            printf("%-40s %d\n", tabla[i].lexema, tabla[i].componente);
            total++;
        }
    }

    printf("========================================\n");
    printf("Total: %d entradas en %d slots\n", total, N);
}

/* ============================================================
 * imprimirPalabrasReservadas
 * ============================================================ */
void imprimirPalabrasReservadas(void) {
    printf("========== PALABRAS RESERVADAS ==========\n");
    for (int i = 0; i < N; i++) {
        if (tabla[i].lexema[0] != VACIO &&
            tabla[i].lexema[0] != BORRADO &&
            tabla[i].componente >= 273 &&
            tabla[i].componente < 300) {
            printf("  %-20s (codigo: %d)\n",
                   tabla[i].lexema, tabla[i].componente);
        }
    }
    printf("=========================================\n\n");
}

/* ============================================================
 * imprimirIdentificadores
 * ============================================================ */
void imprimirIdentificadores(void) {
    printf("\n========== IDENTIFICADORES ==========\n");
    for (int i = 0; i < N; i++) {
        if (tabla[i].lexema[0] != VACIO &&
            tabla[i].lexema[0] != BORRADO &&
            tabla[i].componente == IDENTIFICADOR) {
            printf("  %s\n", tabla[i].lexema);
        }
    }
    printf("=====================================\n");
}
