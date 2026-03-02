/*
 * tabla_simbolos.c
 *
 * Modulo de la tabla de simbolos del analizador lexico.
 *
 * Responsabilidades:
 *   - Almacenar palabras reservadas (pre-cargadas al inicializar)
 *   - Almacenar identificadores encontrados durante el analisis
 *   - Buscar lexemas para distinguir palabras reservadas de identificadores
 *   - Imprimir el contenido de la tabla para depuracion
 *   - Liberar toda la memoria al finalizar (sin punteros colgantes)
 *
 * Implementacion:
 *   Se utiliza un array dinamico (malloc/realloc) que crece
 *   automaticamente cuando se llena, duplicando su capacidad.
 *   Cada lexema se almacena en memoria dinamica (strdup/malloc).
 *   Al finalizar, se libera toda la memoria con liberarTablaSimbolos().
 *
 * Nota: segun los requisitos de la practica, no se pueden usar
 * estructuras estaticas en la version final.
 */

#include "definiciones.h"

/* Variable global de la tabla de simbolos */
TablaSimbolos tablaSimbolos;

/*
 * inicializarTablaSimbolos: reserva memoria inicial para la tabla
 * e inserta todas las palabras reservadas del lenguaje D que
 * aparecen en regression.d.
 *
 * Las palabras reservadas se insertan con linea=0 y columna=0
 * porque no tienen una posicion concreta en el fichero fuente
 * (son parte del lenguaje, no del programa analizado).
 *
 * Tras la inicializacion, se imprimen las palabras reservadas
 * para verificar que se han cargado correctamente.
 */
void inicializarTablaSimbolos(void) {
    tablaSimbolos.cantidad = 0;
    tablaSimbolos.capacidad = CAPACIDAD_INICIAL;

    /* Reservar memoria dinamica para el array de entradas */
    tablaSimbolos.entradas = (EntradaSimbolo *)malloc(
        sizeof(EntradaSimbolo) * tablaSimbolos.capacidad
    );
    if (tablaSimbolos.entradas == NULL) {
        fprintf(stderr, "Error: No se pudo reservar memoria para la tabla de simbolos\n");
        exit(EXIT_FAILURE);
    }

    /* Pre-cargar las palabras reservadas del lenguaje D */
    insertarSimbolo("import",  IMPORT,  0, 0);
    insertarSimbolo("while",   WHILE,   0, 0);
    insertarSimbolo("double",  DOUBLE,  0, 0);
    insertarSimbolo("int",     INT,     0, 0);
    insertarSimbolo("void",    VOID,    0, 0);
    insertarSimbolo("foreach", FOREACH, 0, 0);
    insertarSimbolo("cast",    CAST,    0, 0);
    insertarSimbolo("return",  RETURN,  0, 0);

    /* Imprimir las palabras reservadas para verificar la inicializacion */
    imprimirPalabrasReservadas();
}

/*
 * liberarTablaSimbolos: libera toda la memoria dinamica asociada
 * a la tabla de simbolos.
 *
 * Recorre todas las entradas liberando cada lexema (cadena dinamica)
 * y luego libera el array de entradas. Pone todos los punteros a NULL
 * para evitar punteros colgantes, como exige la practica.
 */
void liberarTablaSimbolos(void) {
    if (tablaSimbolos.entradas != NULL) {
        /* Liberar cada lexema almacenado dinamicamente */
        for (int i = 0; i < tablaSimbolos.cantidad; i++) {
            if (tablaSimbolos.entradas[i].lexema != NULL) {
                free(tablaSimbolos.entradas[i].lexema);
                tablaSimbolos.entradas[i].lexema = NULL;
            }
        }

        /* Liberar el array de entradas */
        free(tablaSimbolos.entradas);
        tablaSimbolos.entradas = NULL;
    }

    tablaSimbolos.cantidad = 0;
    tablaSimbolos.capacidad = 0;
}

/*
 * buscarSimbolo: busca un lexema en la tabla de simbolos.
 *
 * Recorre secuencialmente la tabla comparando cadenas.
 * Se usa tanto para verificar si un identificador ya existe
 * como para distinguir palabras reservadas de identificadores.
 *
 * Parametros:
 *   - lexema: cadena a buscar en la tabla
 *
 * Retorna:
 *   - Indice de la entrada si se encuentra (>= 0)
 *   - -1 si no se encuentra
 */
int buscarSimbolo(const char *lexema) {
    for (int i = 0; i < tablaSimbolos.cantidad; i++) {
        if (strcmp(tablaSimbolos.entradas[i].lexema, lexema) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * insertarSimbolo: inserta un nuevo lexema en la tabla de simbolos.
 *
 * Antes de insertar, verifica si el lexema ya existe para evitar
 * duplicados (especialmente relevante para palabras reservadas
 * e identificadores que aparecen multiples veces).
 *
 * Si la tabla esta llena, duplica su capacidad con realloc.
 * El lexema se copia con strdup (memoria dinamica).
 *
 * Parametros:
 *   - lexema: texto del token a insertar
 *   - componente: codigo numerico del tipo de token
 *   - linea: linea donde aparecio por primera vez
 *   - columna: columna donde aparecio por primera vez
 *
 * Retorna:
 *   - Indice de la entrada (nueva o existente)
 *   - -1 si hubo error de memoria
 */
int insertarSimbolo(const char *lexema, int componente, int linea, int columna) {
    /* Comprobar si ya existe para evitar duplicados */
    int indice = buscarSimbolo(lexema);
    if (indice != -1) {
        return indice;
    }

    /* Si la tabla esta llena, duplicar la capacidad con realloc */
    if (tablaSimbolos.cantidad >= tablaSimbolos.capacidad) {
        int nuevaCapacidad = tablaSimbolos.capacidad * 2;
        EntradaSimbolo *nuevasEntradas = (EntradaSimbolo *)realloc(
            tablaSimbolos.entradas,
            sizeof(EntradaSimbolo) * nuevaCapacidad
        );
        if (nuevasEntradas == NULL) {
            fprintf(stderr, "Error: No se pudo redimensionar la tabla de simbolos\n");
            return -1;
        }
        tablaSimbolos.entradas = nuevasEntradas;
        tablaSimbolos.capacidad = nuevaCapacidad;
    }

    /* Copiar el lexema en memoria dinamica (strdup usa malloc internamente) */
    tablaSimbolos.entradas[tablaSimbolos.cantidad].lexema = strdup(lexema);
    if (tablaSimbolos.entradas[tablaSimbolos.cantidad].lexema == NULL) {
        fprintf(stderr, "Error: No se pudo copiar el lexema '%s'\n", lexema);
        return -1;
    }

    tablaSimbolos.entradas[tablaSimbolos.cantidad].componente = componente;
    tablaSimbolos.entradas[tablaSimbolos.cantidad].linea = linea;
    tablaSimbolos.entradas[tablaSimbolos.cantidad].columna = columna;

    return tablaSimbolos.cantidad++;
}

/*
 * obtenerComponente: devuelve el codigo numerico del token en un indice dado.
 *
 * Parametros:
 *   - indice: posicion en la tabla de simbolos
 *
 * Retorna:
 *   - Codigo del componente lexico, o ERROR_LEXICO si el indice no es valido
 */
int obtenerComponente(int indice) {
    if (indice >= 0 && indice < tablaSimbolos.cantidad) {
        return tablaSimbolos.entradas[indice].componente;
    }
    return ERROR_LEXICO;
}

/*
 * obtenerLexema: devuelve el texto del lexema en un indice dado.
 *
 * Parametros:
 *   - indice: posicion en la tabla de simbolos
 *
 * Retorna:
 *   - Puntero al lexema, o cadena vacia si el indice no es valido
 */
const char *obtenerLexema(int indice) {
    if (indice >= 0 && indice < tablaSimbolos.cantidad) {
        return tablaSimbolos.entradas[indice].lexema;
    }
    return "";
}

/*
 * imprimirTablaSimbolos: muestra todas las entradas de la tabla
 * (palabras reservadas e identificadores) con formato tabular.
 */
void imprimirTablaSimbolos(void) {
    printf("\n========== TABLA DE SIMBOLOS ==========\n");
    printf("%-40s %-15s %s\n", "Lexema", "Componente", "Linea:Columna");
    printf("========================================\n");

    for (int i = 0; i < tablaSimbolos.cantidad; i++) {
        printf("%-40s %-15d %d:%d\n",
               tablaSimbolos.entradas[i].lexema,
               tablaSimbolos.entradas[i].componente,
               tablaSimbolos.entradas[i].linea,
               tablaSimbolos.entradas[i].columna);
    }
    printf("========================================\n");
    printf("Total de entradas: %d\n", tablaSimbolos.cantidad);
}

/*
 * imprimirPalabrasReservadas: muestra solo las palabras reservadas
 * (aquellas con componente entre 273 y 299) de la tabla de simbolos.
 *
 * Se invoca al inicializar la tabla para verificar que las keywords
 * se han cargado correctamente, como exige la practica.
 */
void imprimirPalabrasReservadas(void) {
    printf("========== PALABRAS RESERVADAS ==========\n");
    for (int i = 0; i < tablaSimbolos.cantidad; i++) {
        if (tablaSimbolos.entradas[i].componente >= 273 &&
            tablaSimbolos.entradas[i].componente < 300) {
            printf("  %-20s (codigo: %d)\n",
                   tablaSimbolos.entradas[i].lexema,
                   tablaSimbolos.entradas[i].componente);
        }
    }
    printf("=========================================\n\n");
}

/*
 * imprimirIdentificadores: muestra solo los identificadores
 * (aquellos con componente IDENTIFICADOR) de la tabla de simbolos.
 *
 * Se invoca al final de la ejecucion para verificar que los
 * identificadores se han registrado correctamente durante el analisis.
 */
void imprimirIdentificadores(void) {
    printf("\n========== IDENTIFICADORES ==========\n");
    for (int i = 0; i < tablaSimbolos.cantidad; i++) {
        if (tablaSimbolos.entradas[i].componente == IDENTIFICADOR) {
            printf("  %-20s (linea: %d, columna: %d)\n",
                   tablaSimbolos.entradas[i].lexema,
                   tablaSimbolos.entradas[i].linea,
                   tablaSimbolos.entradas[i].columna);
        }
    }
    printf("=====================================\n");
}
