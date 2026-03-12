#include <stdio.h>
#include <stdlib.h>

#include "AS.h"
#include "AL.h"
#include "SI.h"
#include "definiciones.h"

void inicializar_AS(const char *nombre_fichero) {
    inicializar_SI(nombre_fichero);
}

void analizar(void) {
    ComponenteLexico cl;
    int num_componentes = 0;

    do {
        cl = sig_comp_lexico();
        printf("<%-4d, %s>\n", cl.token, cl.lexema);
        num_componentes++;

        free(cl.lexema);
    } while (cl.token != TOKEN_EOF);

    printf("\nTotal de componentes lexicos: %d\n", num_componentes);
}

void cerrar_AS(void) {
    cerrar_SI();
}