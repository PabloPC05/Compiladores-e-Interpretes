/*
 * errores.c
 *
 * Modulo gestor de errores del analizador lexico.
 *
 * Responsabilidades:
 *   - Reportar errores lexicos indicando la posicion (linea, columna)
 *   - Reportar advertencias (situaciones no criticas)
 *   - Mantener contadores de errores y advertencias
 *   - Imprimir un resumen al final de la ejecucion
 *
 * Los errores se escriben en stderr para separarlos de la salida
 * normal del analizador que va por stdout.
 */

#include "definiciones.h"

/* Contadores internos de errores y advertencias */
static int contadorErrores = 0;
static int contadorAdvertencias = 0;

/*
 * reportarErrorLexico: imprime un error lexico en stderr.
 *
 * Se invoca cuando el automata encuentra un caracter o secuencia
 * que no puede reconocer como ningun componente lexico valido.
 *
 * Parametros:
 *   - linea: numero de linea donde se detecto el error
 *   - columna: numero de columna donde se detecto el error
 *   - mensaje: descripcion del error encontrado
 */
void reportarErrorLexico(int linea, int columna, const char *mensaje) {
    fprintf(stderr, "ERROR LEXICO [Linea %d, Columna %d]: %s\n", linea, columna, mensaje);
    contadorErrores++;
}

/*
 * reportarAdvertencia: imprime una advertencia en stderr.
 *
 * Se usa para situaciones que no impiden el analisis pero
 * merecen atencion (por ejemplo, caracteres inusuales).
 *
 * Parametros:
 *   - linea: numero de linea de la advertencia
 *   - columna: numero de columna de la advertencia
 *   - mensaje: descripcion de la advertencia
 */
void reportarAdvertencia(int linea, int columna, const char *mensaje) {
    fprintf(stderr, "ADVERTENCIA [Linea %d, Columna %d]: %s\n", linea, columna, mensaje);
    contadorAdvertencias++;
}

/*
 * obtenerCantidadErrores: devuelve el total de errores acumulados.
 */
int obtenerCantidadErrores(void) {
    return contadorErrores;
}

/*
 * obtenerCantidadAdvertencias: devuelve el total de advertencias acumuladas.
 */
int obtenerCantidadAdvertencias(void) {
    return contadorAdvertencias;
}

/*
 * reiniciarContadorErrores: pone a cero ambos contadores.
 * Se llama al inicio del analisis para empezar desde un estado limpio.
 */
void reiniciarContadorErrores(void) {
    contadorErrores = 0;
    contadorAdvertencias = 0;
}

/*
 * imprimirResumenErrores: muestra un resumen final con el total
 * de errores y advertencias encontrados durante el analisis.
 */
void imprimirResumenErrores(void) {
    printf("\n========== RESUMEN DE ERRORES ==========\n");
    printf("Total errores: %d\n", contadorErrores);
    printf("Total advertencias: %d\n", contadorAdvertencias);
    printf("=========================================\n");
}
