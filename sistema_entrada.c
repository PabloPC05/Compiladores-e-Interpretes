/*
 * sistema_entrada.c
 *
 * Modulo del sistema de entrada del analizador lexico.
 *
 * Implementa el doble buffer con centinela descrito en el patron
 * productor-consumidor de Aho, Sethi y Ullman:
 *
 *   - Dos bloques estaticos de TAM_BLOQUE caracteres cada uno.
 *   - Cada bloque tiene un centinela '\0' en su ultima posicion.
 *   - Se carga con fread() para mayor eficiencia que getc().
 *   - Carga perezosa: el segundo bloque no se carga hasta que se
 *     agoten los caracteres del primero.
 *   - Puntero delantero: posicion de lectura actual.
 *   - Puntero inicio: comienzo del lexema en curso.
 *
 * Cuando el puntero delantero alcanza el centinela:
 *   - Si bytesEnBloque[mitad] < TAM_BLOQUE => EOF real del fichero.
 *   - Si bytesEnBloque[mitad] == TAM_BLOQUE => fin de bloque, cargar
 *     el siguiente.
 *
 * La funcion moverInicio() sincroniza el puntero inicio con
 * el delantero al comienzo de cada nuevo lexema.
 */

#include "definiciones.h"

/* Variables globales de posicion accesibles desde el analizador */
FILE *archivoFuente = NULL;
int lineaActual = 1;
int columnaActual = 0;

/* ============================================================
 * Doble buffer estatico
 * Cada mitad tiene TAM_BLOQUE caracteres utiles + 1 centinela.
 * ============================================================ */
static unsigned char bloque[2][TAM_BLOQUE + 1];

/* Bytes cargados en cada mitad (< TAM_BLOQUE indica EOF real) */
static int bytesEnBloque[2];

/* Puntero delantero: mitad y posicion actual de lectura */
static int delanteroMitad;
static int delanteroPos;

/* Puntero inicio: comienzo del lexema en curso */
static int inicioMitad;
static int inicioPos;

/* ============================================================
 * cargarBloque: carga TAM_BLOQUE caracteres del fichero en la
 * mitad indicada usando fread() y coloca el centinela al final.
 * ============================================================ */
static void cargarBloque(int mitad) {
    int leidos = (int)fread(bloque[mitad], 1, TAM_BLOQUE, archivoFuente);
    bytesEnBloque[mitad] = leidos;
    bloque[mitad][leidos] = '\0'; /* centinela: marca fin de datos validos */
}

/* ============================================================
 * abrirArchivoFuente: abre el fichero y carga el primer bloque.
 * ============================================================ */
int abrirArchivoFuente(const char *nombreFichero) {
    archivoFuente = fopen(nombreFichero, "r");
    if (archivoFuente == NULL) {
        fprintf(stderr, "Error: No se puede abrir el fichero '%s'\n", nombreFichero);
        return 0;
    }

    lineaActual = 1;
    columnaActual = 0;

    /* Carga inicial: solo el primer bloque (carga perezosa) */
    cargarBloque(0);
    bytesEnBloque[1] = 0;

    delanteroMitad = 0;
    delanteroPos   = 0;
    inicioMitad    = 0;
    inicioPos      = 0;

    return 1;
}

/* ============================================================
 * cerrarArchivoFuente: cierra el fichero y pone el puntero a NULL.
 * ============================================================ */
void cerrarArchivoFuente(void) {
    if (archivoFuente != NULL) {
        fclose(archivoFuente);
        archivoFuente = NULL;
    }
}

/* ============================================================
 * siguienteCaracter: devuelve el siguiente caracter del fichero.
 *
 * Avanza el puntero delantero. Cuando alcanza el centinela:
 *   - Si era el fin real del fichero, retorna EOF.
 *   - Si era el fin de bloque, carga la otra mitad y continua.
 *
 * Actualiza lineaActual y columnaActual.
 * ============================================================ */
int siguienteCaracter(void) {
    /* Si delantero esta al final de los datos validos de este bloque */
    if (delanteroPos >= bytesEnBloque[delanteroMitad]) {
        if (bytesEnBloque[delanteroMitad] < TAM_BLOQUE) {
            /* EOF real: no hay mas datos en el fichero */
            return EOF;
        }
        /* Fin de bloque: cargar la otra mitad de forma perezosa */
        int otraMitad = 1 - delanteroMitad;
        cargarBloque(otraMitad);
        delanteroMitad = otraMitad;
        delanteroPos   = 0;

        /* Comprobar si la nueva mitad esta vacia (EOF inmediato) */
        if (bytesEnBloque[delanteroMitad] == 0) {
            return EOF;
        }
    }

    unsigned char c = bloque[delanteroMitad][delanteroPos++];

    /* Actualizar posicion en el fichero fuente */
    columnaActual++;
    if (c == '\n') {
        lineaActual++;
        columnaActual = 0;
    }

    return (int)c;
}

/* ============================================================
 * devolverCaracter: retrocede el puntero delantero un caracter.
 *
 * El automata lo usa para "devolver" el caracter de lookahead
 * que no pertenece al lexema actual. Solo se permite retroceder
 * un caracter (lookahead = 1).
 *
 * Ajusta lineaActual y columnaActual para compensar.
 * ============================================================ */
void devolverCaracter(int c) {
    if (c == EOF) return;

    if (delanteroPos > 0) {
        delanteroPos--;
    } else {
        /*
         * El delantero esta al inicio de la mitad actual.
         * Retroceder a la mitad anterior (la que fue desplazada).
         * Como solo hay lookahead de 1, esto solo ocurre al cruzar
         * exactamente el limite entre dos bloques.
         */
        int mitadAnterior = 1 - delanteroMitad;
        delanteroMitad = mitadAnterior;
        delanteroPos   = bytesEnBloque[mitadAnterior] - 1;
    }

    /* Compensar la posicion que se avanzo al leer este caracter */
    if (c == '\n') {
        lineaActual--;
    } else {
        columnaActual--;
    }
}

/* ============================================================
 * moverInicio: desplaza el puntero inicio hasta la posicion
 * actual del delantero, marcando el comienzo de un nuevo lexema.
 *
 * El analizador lexico debe llamar a esta funcion justo antes
 * de empezar a leer cada nuevo componente lexico.
 * ============================================================ */
void moverInicio(void) {
    inicioMitad = delanteroMitad;
    inicioPos   = delanteroPos;
}
