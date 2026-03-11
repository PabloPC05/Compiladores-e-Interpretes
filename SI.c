#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SI.h"
#include "definiciones.h"

static char buf[2][TAM_BUFFER + 1];
static int bloque_activo;
static int delantero;
static int inicio;
static int bloque_inicio;
static FILE *fichero = NULL;

static void cargar_bloque(int b) {
    int leidos = (int)fread(buf[b], sizeof(char), TAM_BUFFER, fichero);
    buf[b][leidos] = EOF;
}

void inicializar_SI(const char *nombre_fichero) {
    fichero = fopen(nombre_fichero, "r");
    if (!fichero) {
        fprintf(stderr, "Error: no se puede abrir el fichero '%s'\n", nombre_fichero);
        exit(1);
    }

    bloque_activo = 0;
    bloque_inicio = 0;
    delantero = 0;
    inicio = 0;

    cargar_bloque(0);
}

char sig_caracter(void) {
    char c = buf[bloque_activo][delantero];

    if (c != (char)EOF) {
        delantero++;
        return c;
    }

    if (feof(fichero))
        return (char)EOF;

    // Fin de bloque: cargar el siguiente
    bloque_activo = 1 - bloque_activo;
    cargar_bloque(bloque_activo);
    delantero = 0;

    c = buf[bloque_activo][delantero];
    if (c == (char)EOF)
        return (char)EOF;

    delantero++;
    return c;
}

/*char peek_caracter(void) {
    char c = buf[bloque_activo][delantero];

    if (c != (char)EOF)
        return c;

    if (feof(fichero))
        return (char)EOF;

    // Cuando llegamos al final del bloque cargamos el siguiente
    bloque_activo = 1 - bloque_activo;
    cargar_bloque(bloque_activo);
    delantero = 0;

    return buf[bloque_activo][delantero];
}*/

void devolver_caracter(void) {
    if (delantero > 0) {
        delantero--;
    } else { // Hay que volver al bloque anterior
        bloque_activo = 1 - bloque_activo;
        delantero = TAM_BUFFER - 1;
    }
}

char *get_lexema(void) {
    int longitud;

    if (bloque_inicio == bloque_activo) {
        longitud = delantero - inicio;
        char *lexema = malloc(longitud + 1);
        if (!lexema) {
            fprintf(stderr, "Error: memoria insuficiente en get_lexema\n");
            exit(1);
        }
        strncpy(lexema, buf[bloque_inicio] + inicio, longitud);
        lexema[longitud] = '\0';
        return lexema;

    } else { // El lexema cruza la frontera entre bloques 
        int parte1 = TAM_BUFFER - inicio;
        int parte2 = delantero;
        longitud = parte1 + parte2;
        char *lexema = malloc(longitud + 1);
        if (!lexema) {
            fprintf(stderr, "Error: memoria insuficiente en get_lexema\n");
            exit(1);
        }
        strncpy(lexema, buf[bloque_inicio] + inicio, parte1);
        strncpy(lexema + parte1, buf[bloque_activo], parte2);
        lexema[longitud] = '\0';
        return lexema;
    }
}

void mover_inicio(void) {
    bloque_inicio = bloque_activo;
    inicio = delantero;
}

void cerrar_SI(void) {
    if (fichero) {
        fclose(fichero);
        fichero = NULL;
    }
}
