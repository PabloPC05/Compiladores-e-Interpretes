#include <stdio.h>
#include <stdlib.h>
#include "errores.h"

void report(const char *msg, int linea, int columna, int fatal) {
    if (linea > 0)
        fprintf(stderr, "Error [%d:%d]: %s\n", linea, columna, msg);
    else
        fprintf(stderr, "Error: %s\n", msg);

    if (fatal) exit(1);
}
