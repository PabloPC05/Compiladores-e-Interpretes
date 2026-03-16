#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SI.h"
#include "definiciones.h"
#include "errores.h"

typedef struct {
    char  buf[2][TAM_BUFFER + 1];
    int   bloque_activo;
    int   bloque_inicio;
    char *delantero;
    char *inicio;
    FILE *fichero;
    int   linea;
    int   columna;
    int   columna_anterior;
} SistemaEntrada;

static SistemaEntrada si = { .fichero = NULL, .linea = 1, .columna = 0, .columna_anterior = 0 };

static void cargar_bloque(int b) {
    int leidos = (int)fread(si.buf[b], sizeof(char), TAM_BUFFER, si.fichero);
    si.buf[b][leidos] = '\0'; // Añadimos el centinela para detectar fin de bloque
}

void inicializar_SI(const char *nombre_fichero) {
    si.fichero = fopen(nombre_fichero, "r");
    if (!si.fichero) {
        fatal(ERR_FICHERO_NO_ABIERTO);
    }

    si.bloque_activo = 0;
    si.bloque_inicio = 0;
    si.delantero = si.buf[0];
    si.inicio = si.buf[0];
    si.linea = 1;
    si.columna = 0;
    si.columna_anterior = 0;

    cargar_bloque(0);
}

// Devuelve el siguiente caracter como un entero (o EOF) para evitar ambigüedades con caracteres no ASCII
int sig_caracter(void) {
    // Si el caracter actual es el centinela, debemos cargar el siguiente bloque o devolver EOF si ya no hay más caracteres
    if (*si.delantero == '\0') {
        if (feof(si.fichero)) return EOF;

        si.bloque_activo = 1 - si.bloque_activo; // Cambiamos el bloque activo
        cargar_bloque(si.bloque_activo); // Cargamos el nuevo bloque
        si.delantero = si.buf[si.bloque_activo]; // Apuntamos al inicio del nuevo bloque

        // Si el puntero de inicio quedó apuntando a una posición en la que ya no es útil (un caracter real) lo movemos al inicio del nuevo bloque
        if (si.inicio == &si.buf[1 - si.bloque_activo][TAM_BUFFER]) 
            si.inicio = si.buf[si.bloque_activo];

        if (*si.delantero == '\0') return EOF;
    }

    int c = (unsigned char)*si.delantero;

    // Actualizamos las coordenadas
    if (c == '\n') {
        si.linea++;
        si.columna_anterior = si.columna;
        si.columna = 0;
    } else {
        si.columna++;
    }

    si.delantero++;
    return c;
}

// Retrocede un caracter, actualizando coordenadas. Protegido para no retroceder más allá del inicio del lexema.
void devolver_caracter(void) {

    // Si el puntero de delantero ya apunta al inicio del lexema, no podemos retroceder más
    if (si.delantero == si.inicio) return;

    // Si el puntero de delantero apunta al inicio del bloque activo, al retroceder debemos cambiar de bloque
    if (si.delantero == si.buf[si.bloque_activo]) {
        // Cambiamos el bloque activo
        si.bloque_activo = 1 - si.bloque_activo;
        // Colocamos el puntero de delantero al final del bloque anterior
        si.delantero = &si.buf[si.bloque_activo][TAM_BUFFER - 1];
    } else { // Si aun tenemos espacio de retroceso, simplemente retrocedemos el puntero de delantero
        si.delantero--;
    }

    // Actualizamos las coordenadas en el código
    if (*si.delantero == '\n') {
        si.linea--;
        si.columna = si.columna_anterior;
    } else if (si.columna > 0) {
        si.columna--;
    }
}

/* Devuelve el lexema de los buffers entre inicio y delantero */
char *get_lexema(void) {
    int n1, n2;

    // Si el bloque de inicio y el activo son el mismo y el puntero de delantero está por delante del de inicio, el lexema no cruza la frontera entre bloques y podemos copiarlo directamente
    if (si.bloque_inicio == si.bloque_activo && si.delantero >= si.inicio) {
        n1 = (int)(si.delantero - si.inicio);
        n2 = 0;
    } else {
        // El lexema cruza la frontera entre bloques
        n1 = (int)(&si.buf[si.bloque_inicio][TAM_BUFFER] - si.inicio);
        n2 = (int)(si.delantero - si.buf[si.bloque_activo]);
    }

    char *lexema = malloc(n1 + n2 + 1); // Reservamos memoria para el lexema + '\0'
    if (!lexema) fatal(ERR_MEMORIA_INSUFICIENTE);

    memcpy(lexema, si.inicio, n1);

    // Si además hay parte del lexema en el siguiente bloque, copiarla también
    if (n2 > 0) memcpy(lexema + n1, si.buf[si.bloque_activo], n2);

    lexema[n1 + n2] = '\0';
    return lexema;
}

void mover_inicio(void) {
    si.bloque_inicio = si.bloque_activo;
    si.inicio = si.delantero;
}

int obtener_linea(void)   { 
    return si.linea; 
}
int obtener_columna(void) { 
    return si.columna; 
}

void cerrar_SI(void) {
    if (si.fichero) {
        fclose(si.fichero);
        si.fichero = NULL;
    }
}
