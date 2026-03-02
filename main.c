/*
 * main.c
 *
 * Programa principal del analizador lexico para el lenguaje D.
 *
 * Estructura de ejecucion en tres fases:
 *
 *   1. INICIALIZACION:
 *      - Abrir el fichero fuente (.d)
 *      - Inicializar la tabla de simbolos (carga las palabras reservadas)
 *      - Reiniciar los contadores de errores
 *
 *   2. ANALISIS LEXICO:
 *      - Bucle principal: el main actua como analizador sintactico
 *        simplificado (consumidor) que llama repetidamente a
 *        siguienteComponenteLexico() (productor) hasta recibir
 *        el token FIN_DE_FICHERO.
 *      - Cada componente lexico se imprime con su posicion y tipo.
 *
 *   3. FINALIZACION:
 *      - Imprimir la tabla de simbolos completa
 *      - Imprimir los identificadores encontrados
 *      - Imprimir el resumen de errores
 *      - Cerrar el fichero fuente
 *      - Liberar toda la memoria dinamica (sin punteros colgantes)
 *
 * Uso:
 *   ./analizador                  (analiza regression.d por defecto)
 *   ./analizador fichero.d        (analiza el fichero especificado)
 */

#include "definiciones.h"

/*
 * nombreComponente: traduce el codigo numerico de un componente lexico
 * a su nombre legible para la salida por pantalla.
 *
 * Parametros:
 *   - componente: codigo numerico del token
 *
 * Retorna: cadena constante con el nombre del componente.
 */
static const char *nombreComponente(int componente) {
    switch (componente) {
        /* Palabras reservadas */
        case IMPORT:          return "IMPORT";
        case WHILE:           return "WHILE";
        case DOUBLE:          return "DOUBLE";
        case INT:             return "INT";
        case VOID:            return "VOID";
        case FOREACH:         return "FOREACH";
        case CAST:            return "CAST";
        case RETURN:          return "RETURN";

        /* Operadores y signos de puntuacion */
        case PUNTO_Y_COMA:    return "PUNTO_Y_COMA";
        case PARENTESIS_IZQ:  return "PARENTESIS_IZQ";
        case PARENTESIS_DER:  return "PARENTESIS_DER";
        case LLAVE_IZQ:       return "LLAVE_IZQ";
        case LLAVE_DER:       return "LLAVE_DER";
        case CORCHETE_IZQ:    return "CORCHETE_IZQ";
        case CORCHETE_DER:    return "CORCHETE_DER";
        case COMA:            return "COMA";
        case PUNTO:           return "PUNTO";
        case ASIGNACION:      return "ASIGNACION";
        case SUMA:            return "SUMA";
        case RESTA:           return "RESTA";
        case MULTIPLICACION:  return "MULTIPLICACION";
        case DIVISION:        return "DIVISION";
        case IGUAL:           return "IGUAL";
        case MENOR:           return "MENOR";
        case SUMA_ASIGNACION: return "SUMA_ASIGNACION";
        case INCREMENTO:      return "INCREMENTO";

        /* Literales */
        case NUMERO:          return "NUMERO";
        case NUMERO_DECIMAL:  return "NUMERO_DECIMAL";
        case IDENTIFICADOR:   return "IDENTIFICADOR";
        case CADENA:          return "CADENA";

        /* Especiales */
        case FIN_DE_FICHERO:  return "FIN_DE_FICHERO";
        case ERROR_LEXICO:    return "ERROR_LEXICO";
        default:              return "DESCONOCIDO";
    }
}

int main(int argc, char *argv[]) {
    const char *nombreFichero = "regression.d";

    /* Permitir especificar un fichero diferente por argumento */
    if (argc > 1) {
        nombreFichero = argv[1];
    }

    /* ========== FASE 1: INICIALIZACION ========== */

    /* Abrir el fichero fuente */
    if (!abrirArchivoFuente(nombreFichero)) {
        return 1;
    }

    /* Inicializar la tabla de simbolos con las palabras reservadas */
    inicializarTablaSimbolos();

    /* Reiniciar contadores de errores */
    reiniciarContadorErrores();

    /* ========== FASE 2: ANALISIS LEXICO ========== */

    printf("========== ANALISIS LEXICO ==========\n");
    printf("Fichero: %s\n", nombreFichero);
    printf("=====================================\n\n");

    char lexema[MAX_LONGITUD_LEXEMA];
    int componente;
    int contadorTokens = 0;

    /*
     * Bucle principal: el main (consumidor) pide componentes lexicos
     * al analizador lexico (productor) mediante siguienteComponenteLexico().
     * Cada llamada devuelve un struct conceptual <componente, lexema>.
     * El bucle termina cuando se recibe FIN_DE_FICHERO (EOF del fichero).
     */
    while ((componente = siguienteComponenteLexico(lexema)) != FIN_DE_FICHERO) {
        if (componente == ERROR_LEXICO) {
            continue; /* Saltar errores y seguir analizando */
        }

        printf("[%3d] Linea %d, Columna %d: Componente=%-20s Lexema='%s'\n",
               ++contadorTokens, lineaActual, columnaActual,
               nombreComponente(componente), lexema);
    }

    /* ========== FASE 3: FINALIZACION ========== */

    /* Imprimir la tabla de simbolos completa */
    imprimirTablaSimbolos();

    /* Imprimir los identificadores encontrados durante el analisis */
    imprimirIdentificadores();

    /* Imprimir resumen de errores */
    imprimirResumenErrores();

    /* Cerrar el fichero fuente */
    cerrarArchivoFuente();

    /* Liberar toda la memoria de la tabla de simbolos (sin punteros colgantes) */
    liberarTablaSimbolos();

    return obtenerCantidadErrores() > 0 ? 1 : 0;
}
