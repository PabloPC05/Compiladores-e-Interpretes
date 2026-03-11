#include <stdio.h>
#include <stdlib.h>

#include "definiciones.h"
#include "TS.h"
#include "SI.h"
#include "AL.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <fichero.d>\n", argv[0]);
        return 1;
    }

    // Inicializacion
    inicializar_TS();
    inicializar_SI(argv[1]);

    // Analisis lexico
    ComponenteLexico cl;
    int num_componentes = 0;

    do {
        cl = sig_comp_lexico();
        printf("<%-4d, %s>\n", cl.token, cl.lexema);
        num_componentes++;
        free(cl.lexema);
    } while (cl.token != TOKEN_EOF);

    printf("\nTotal de componentes lexicos: %d\n\n", num_componentes);

    // Finalizacion 
    imprimir_TS();
    liberar_TS();
    cerrar_SI();

    return 0;
}
