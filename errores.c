#include <stdio.h>
#include <stdlib.h>
#include "errores.h"

void report(int linea, int columna, const char *msg) {
    fprintf(stderr, "Error lexico [%d:%d]: %s\n", linea, columna, msg);
}

void fatal(const char *msg) {
    fprintf(stderr, "Error fatal: %s\n", msg);
    exit(EXIT_FAILURE);
}
