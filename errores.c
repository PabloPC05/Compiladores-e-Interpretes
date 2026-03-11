#include <stdio.h>
#include "errores.h"

void report(char c) {
    fprintf(stderr, "Error lexico: caracter no reconocido '%c' (ASCII %d)\n", c, (unsigned char)c);
}
