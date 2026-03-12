#include <stdio.h>
#include <stdlib.h>

#include "definiciones.h"
#include "TS.h"
#include "AS.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <fichero.d>\n", argv[0]);
        return 1;
    }

    // Inicializacion de la tabla de simbolos e impresion con palabras reservadas
    inicializar_TS();
    printf("=== Tabla de simbolos inicial (palabras reservadas) ===\n");
    imprimir_TS();
    printf("\n");

    // Inicializacion y ejecucion del analizador sintactico
    inicializar_AS(argv[1]);
    printf("=== Analisis ===\n");
    analizar();
    printf("\n");

    // Impresion final de la tabla de simbolos y limpieza
    printf("=== Tabla de simbolos final ===\n");
    imprimir_TS();
    liberar_TS();
    cerrar_AS();

    return 0;
}
