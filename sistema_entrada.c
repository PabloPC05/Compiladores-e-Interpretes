/*
 * sistema_entrada.c
 *
 * Modulo del sistema de entrada del analizador lexico.
 *
 * Responsabilidades:
 *   - Abrir y cerrar el fichero fuente
 *   - Proporcionar caracteres uno a uno al analizador lexico
 *   - Mantener el seguimiento de la posicion (linea y columna)
 *   - Permitir devolver un caracter al buffer cuando el automata
 *     ha leido un caracter de mas (lookahead de 1 caracter)
 *
 * El analizador lexico consume caracteres mediante siguienteCaracter()
 * y, cuando el automata lee un caracter que no pertenece al lexema
 * actual, lo devuelve con devolverCaracter() para que sea procesado
 * en la siguiente llamada.
 */

#include "definiciones.h"

/* Variables globales de posicion en el fichero fuente */
FILE *archivoFuente = NULL;
int lineaActual = 1;
int columnaActual = 0;

/* Buffer interno para lookahead de 1 caracter */
static int bufferCaracter = EOF;
static int bufferActivo = 0;

/*
 * abrirArchivoFuente: abre el fichero fuente para lectura.
 *
 * Parametros:
 *   - nombreFichero: ruta al fichero .d a analizar
 *
 * Retorna:
 *   - 1 si se abrio correctamente
 *   - 0 si hubo un error al abrir el fichero
 *
 * Inicializa la posicion a linea 1, columna 0.
 */
int abrirArchivoFuente(const char *nombreFichero) {
    archivoFuente = fopen(nombreFichero, "r");
    if (archivoFuente == NULL) {
        fprintf(stderr, "Error: No se puede abrir el fichero '%s'\n", nombreFichero);
        return 0;
    }
    lineaActual = 1;
    columnaActual = 0;
    return 1;
}

/*
 * cerrarArchivoFuente: cierra el fichero fuente y libera el recurso.
 * Pone el puntero a NULL para evitar punteros colgantes.
 */
void cerrarArchivoFuente(void) {
    if (archivoFuente != NULL) {
        fclose(archivoFuente);
        archivoFuente = NULL;
    }
}

/*
 * siguienteCaracter: devuelve el siguiente caracter del fichero fuente.
 *
 * Si hay un caracter en el buffer de lookahead, lo devuelve primero.
 * En caso contrario, lee del fichero con getc().
 *
 * Actualiza la posicion (linea y columna) segun el caracter leido:
 *   - '\n' incrementa la linea y reinicia la columna a 0
 *   - Cualquier otro caracter incrementa la columna
 *   - EOF no modifica la posicion
 *
 * Retorna: el caracter leido, o EOF si se llego al final del fichero.
 */
int siguienteCaracter(void) {
    int c;

    if (bufferActivo) {
        /* Hay un caracter devuelto previamente: consumirlo */
        c = bufferCaracter;
        bufferActivo = 0;
    } else {
        /* Leer directamente del fichero fuente */
        c = getc(archivoFuente);
    }

    /* Actualizar posicion solo si no es EOF */
    if (c != EOF) {
        columnaActual++;
        if (c == '\n') {
            lineaActual++;
            columnaActual = 0;
        }
    }

    return c;
}

/*
 * devolverCaracter: devuelve un caracter al buffer de lookahead.
 *
 * Se usa cuando el automata ha leido un caracter que no pertenece
 * al lexema actual y necesita "deshacerlo" para que sea procesado
 * como parte del siguiente lexema.
 *
 * Solo permite devolver un caracter a la vez (lookahead = 1).
 * Ajusta la posicion (linea/columna) para compensar la lectura.
 *
 * Parametros:
 *   - c: caracter a devolver (se ignora si es EOF o si el buffer
 *     ya esta activo)
 */
void devolverCaracter(int c) {
    if (!bufferActivo && c != EOF) {
        bufferCaracter = c;
        bufferActivo = 1;

        /* Compensar la posicion que se avanzo al leer este caracter */
        if (c == '\n') {
            lineaActual--;
        } else {
            columnaActual--;
        }
    }
}
