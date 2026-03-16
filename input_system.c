#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_system.h"
#include "definitions.h"
#include "errors.h"

typedef struct {
    char  buf[2][BUFFER_SIZE + 1];
    int   activeBlock;
    int   inicioBlock;
    char *delantero;
    char *inicio;
    FILE *file;
    int   line;
    int   column;
    int   prevColumn;
} InputSystem;

static InputSystem inputSystem = { .file = NULL, .line = 1, .column = 0, .prevColumn = 0 };

static void loadBlock(int b) {
    int n = (int)fread(inputSystem.buf[b], sizeof(char), BUFFER_SIZE, inputSystem.file);
    inputSystem.buf[b][n] = '\0'; // Añadimos el centinela para detectar fin de bloque
}

void initInput(const char *filename) {
    inputSystem.file = fopen(filename, "r");
    if (!inputSystem.file) {
        fatal(ERR_FILE_NOT_OPENED);
    }

    inputSystem.activeBlock = 0;
    inputSystem.inicioBlock = 0;
    inputSystem.delantero = inputSystem.buf[0];
    inputSystem.inicio = inputSystem.buf[0];
    inputSystem.line = 1;
    inputSystem.column = 0;
    inputSystem.prevColumn = 0;

    loadBlock(0);
}

// Devuelve el siguiente caracter como un entero (o EOF) para evitar ambigüedades con caracteres no ASCII
int nextChar() {
    // Si el caracter actual es el centinela, debemos cargar el siguiente bloque o devolver EOF si ya no hay más caracteres
    if (*inputSystem.delantero == '\0') {
        if (feof(inputSystem.file)) return EOF;

        inputSystem.activeBlock = 1 - inputSystem.activeBlock; // Cambiamos el bloque activo
        loadBlock(inputSystem.activeBlock); // Cargamos el nuevo bloque
        inputSystem.delantero = inputSystem.buf[inputSystem.activeBlock]; // Apuntamos al inicio del nuevo bloque

        // Si el puntero de inicio quedó apuntando a una posición en la que ya no es útil (un caracter real) lo movemos al inicio del nuevo bloque
        if (inputSystem.inicio == &inputSystem.buf[1 - inputSystem.activeBlock][BUFFER_SIZE])
            inputSystem.inicio = inputSystem.buf[inputSystem.activeBlock];

        if (*inputSystem.delantero == '\0') return EOF;
    }

    int c = (unsigned char)*inputSystem.delantero;

    // Actualizamos las coordenadas
    if (c == '\n') {
        inputSystem.line++;
        inputSystem.prevColumn = inputSystem.column;
        inputSystem.column = 0;
    } else {
        inputSystem.column++;
    }

    inputSystem.delantero++;
    return c;
}

// Retrocede un caracter, actualizando coordenadas. Protegido para no retroceder más allá del inicio del lexema.
void returnChar() {

    // Si el puntero de delantero ya apunta al inicio del lexema, no podemos retroceder más
    if (inputSystem.delantero == inputSystem.inicio) return;

    // Si el puntero de delantero apunta al inicio del bloque activo, al retroceder debemos cambiar de bloque
    if (inputSystem.delantero == inputSystem.buf[inputSystem.activeBlock]) {
        // Cambiamos el bloque activo
        inputSystem.activeBlock = 1 - inputSystem.activeBlock;
        // Colocamos el puntero de delantero al final del bloque anterior
        inputSystem.delantero = &inputSystem.buf[inputSystem.activeBlock][BUFFER_SIZE - 1];
    } else { // Si aun tenemos espacio de retroceso, simplemente retrocedemos el puntero de delantero
        inputSystem.delantero--;
    }

    // Actualizamos las coordenadas en el código
    if (*inputSystem.delantero == '\n') {
        inputSystem.line--;
        inputSystem.column = inputSystem.prevColumn;
    } else if (inputSystem.column > 0) {
        inputSystem.column--;
    }
}

/* Devuelve el lexema de los buffers entre inicio y delantero */
char *getLexeme() {
    int n1, n2;

    // Si el bloque de inicio y el activo son el mismo y el puntero de delantero está por delante del de inicio, el lexema no cruza la frontera entre bloques y podemos copiarlo directamente
    if (inputSystem.inicioBlock == inputSystem.activeBlock && inputSystem.delantero >= inputSystem.inicio) {
        n1 = (int)(inputSystem.delantero - inputSystem.inicio);
        n2 = 0;
    } else {
        // El lexema cruza la frontera entre bloques
        n1 = (int)(&inputSystem.buf[inputSystem.inicioBlock][BUFFER_SIZE] - inputSystem.inicio);
        n2 = (int)(inputSystem.delantero - inputSystem.buf[inputSystem.activeBlock]);
    }

    char *lexeme = malloc(n1 + n2 + 1); // Reservamos memoria para el lexema + '\0'
    if (!lexeme) fatal(ERR_OUT_OF_MEMORY);

    memcpy(lexeme, inputSystem.inicio, n1);

    // Si además hay parte del lexema en el siguiente bloque, copiarla también
    if (n2 > 0) memcpy(lexeme + n1, inputSystem.buf[inputSystem.activeBlock], n2);

    lexeme[n1 + n2] = '\0';
    return lexeme;
}

void moveInicio() {
    inputSystem.inicioBlock = inputSystem.activeBlock;
    inputSystem.inicio = inputSystem.delantero;
}

int getLine()   {
    return inputSystem.line;
}
int getColumn() {
    return inputSystem.column;
}

void closeInput() {
    if (inputSystem.file) {
        fclose(inputSystem.file);
        inputSystem.file = NULL;
    }
}
