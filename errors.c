#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

void report(int line, int column, const char *msg) {
    fprintf(stderr, "Error lexico [%d:%d]: %s\n", line, column, msg);
}

void fatal(const char *msg) {
    fprintf(stderr, "Error fatal: %s\n", msg);
    exit(EXIT_FAILURE);
}
