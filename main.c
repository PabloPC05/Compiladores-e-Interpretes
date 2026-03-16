#include <stdio.h>

#include "definitions.h"
#include "symbol_table.h"
#include "analyzer.h"
#include "errors.h"

int main(int argc, char *argv[]) {
    if (argc < 2) fatal("uso: analizador <fichero.d>");

    // Inicializacion de la tabla de simbolos e impresion con palabras reservadas
    initSymbolTable();
    printf("=== Tabla de simbolos inicial (palabras reservadas) ===\n");
    printSymbolTable();
    printf("\n");

    // Inicializacion y ejecucion del analizador sintactico
    initAnalyzer(argv[1]);
    printf("=== Analisis ===\n");
    analyze();
    printf("\n");

    // Impresion final de la tabla de simbolos y limpieza
    printf("=== Tabla de simbolos final ===\n");
    printSymbolTable();
    freeSymbolTable();
    closeAnalyzer();

    return 0;
}
