#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SI.h"
#include "definiciones.h"
#include "errores.h"

typedef struct {
    char  buf[2][TAM_BUFFER + 1];
    int   bloque_activo;
    int   delantero;
    int   inicio;
    int   bloque_inicio;
    FILE *fichero;
} SistemaEntrada;

static SistemaEntrada si = { .fichero = NULL };

static void cargar_bloque(int b) {
    int leidos = (int)fread(si.buf[b], sizeof(char), TAM_BUFFER, si.fichero);
    si.buf[b][leidos] = EOF;
}

void inicializar_SI(const char *nombre_fichero) {
    si.fichero = fopen(nombre_fichero, "r");
    if (!si.fichero) {
        report(ERR_FICHERO_NO_ABIERTO, 0, 0, 1);
    }

    si.bloque_activo = 0;
    si.bloque_inicio = 0;
    si.delantero = 0;
    si.inicio = 0;

    cargar_bloque(0);
}

char sig_caracter(void) {
    char c = si.buf[si.bloque_activo][si.delantero];

    if (c != (char)EOF) {
        si.delantero++;
        return c;
    }

    if (feof(si.fichero))
        return (char)EOF;

    // Fin de bloque: cargar el siguiente
    si.bloque_activo = 1 - si.bloque_activo;
    cargar_bloque(si.bloque_activo);
    si.delantero = 0;

    c = si.buf[si.bloque_activo][si.delantero];
    if (c == (char)EOF)
        return (char)EOF;

    si.delantero++;
    return c;
}

void devolver_caracter(void) {
    if (si.delantero > 0) {
        si.delantero--;
    } else { // Hay que volver al bloque anterior
        si.bloque_activo = 1 - si.bloque_activo;
        si.delantero = TAM_BUFFER - 1;
    }
}

// Devuelve el lexema de los buffers entre inicio y delantero 
char *get_lexema(void) {
    // Calculamos la longitud del lexema a copiar, teniendo en cuenta que el buffer es circular
    // Para ello contamos con que puede tener dos partes: 
    // Parte 1: desde inicio hasta el final del bloque_inicio: TAM_BUFFER - inicio
    // Parte 2: desde el inicio del bloque_activo hasta delantero: delantero
    int longitud = si.bloque_activo == si.bloque_inicio
                    ? si.delantero - si.inicio
                    : (TAM_BUFFER - si.inicio) + si.delantero;

    // Reservamos el especio en memorria necesario para guardar el lexema
    char *lexema = malloc(longitud + 1);
    if (!lexema) {
        report(ERR_MEMORIA_INSUFICIENTE, 0, 0, 1);
    }

    // Copiamos los datos del buffer al puntero char del lexema
    if (si.bloque_inicio == si.bloque_activo) {
        memcpy(lexema, si.buf[si.bloque_inicio] + si.inicio, longitud);
    } else {
        memcpy(lexema, si.buf[si.bloque_inicio] + si.inicio, TAM_BUFFER - si.inicio);
        memcpy(lexema + (TAM_BUFFER - si.inicio), si.buf[si.bloque_activo], si.delantero);
    }

    lexema[longitud] = '\0'; // Aseguramos que el lexema es una cadena de caracteres válida
    return lexema;
}

void mover_inicio(void) {
    si.bloque_inicio = si.bloque_activo;
    si.inicio = si.delantero;
}

void cerrar_SI(void) {
    if (si.fichero) {
        fclose(si.fichero);
        si.fichero = NULL;
    }
}
