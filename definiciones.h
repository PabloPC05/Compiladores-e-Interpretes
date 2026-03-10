#ifndef DEFINICIONES_H
#define DEFINICIONES_H

/*
 * definiciones.h
 *
 * Archivo de cabecera principal del analizador lexico.
 * Contiene las definiciones de tokens, constantes globales
 * y prototipos de funciones utilizados por todos los modulos.
 *
 * Rangos de codigos de tokens:
 *   - 273-299: Palabras reservadas del lenguaje D
 *   - 300-349: Operadores y signos de puntuacion
 *   - 350-399: Literales (numeros, cadenas, identificadores)
 *   - 400+:    Tokens especiales (EOF, errores)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ============================================================
 * Constantes del sistema de entrada (doble buffer)
 *
 * TAM_BLOQUE: tamano de cada mitad del doble buffer.
 *   El valor ideal es el total de caracteres del fichero fuente,
 *   pero 1024 es un buen punto de partida para pruebas.
 *   Cambiar este valor permite probar distintos escenarios.
 *
 * MAX_LONGITUD_ID: longitud maxima permitida para un identificador.
 *   Debe ser <= TAM_BLOQUE para evitar que un identificador abarque
 *   mas de un bloque (lo que causaria un error de lectura).
 *
 * MAX_LONGITUD_LEXEMA: tamano del buffer de lexema en el analizador.
 *   Cubre cadenas y numeros que pueden ser mas largos que un ID.
 * ============================================================ */
#define TAM_BLOQUE        1024
#define MAX_LONGITUD_ID   64
#define MAX_LONGITUD_LEXEMA 256

/* ============================================================
 * Constante de la tabla hash (tabla_simbolos.c / TS.c)
 *
 * TAM_HASH: numero de slots de la tabla. Debe ser primo y
 * al menos el doble del numero esperado de simbolos para
 * mantener un factor de carga bajo (< 50%).
 * Con ~50 simbolos en regression.d, 509 es mas que suficiente.
 * ============================================================ */
#define TAM_HASH 509

/* ============================================================
 * Codigos de palabras reservadas del lenguaje D (273-299)
 * ============================================================ */
#define IMPORT    273
#define WHILE     274
#define DOUBLE    275
#define INT       276
#define VOID      277
#define FOREACH   278
#define CAST      279
#define RETURN    280
#define ENFORCE   281
#define IF        282
#define ELSE      283
#define FOR       284
#define DO        285
#define BREAK     286
#define CONTINUE  287
#define SWITCH    288
#define CASE      289
#define DEFAULT   290

/* ============================================================
 * Codigos de operadores y signos de puntuacion (300-349)
 * ============================================================ */
#define PUNTO_Y_COMA    300
#define PARENTESIS_IZQ  301
#define PARENTESIS_DER  302
#define LLAVE_IZQ       303
#define LLAVE_DER       304
#define CORCHETE_IZQ    305
#define CORCHETE_DER    306
#define COMA            307
#define PUNTO           308
#define ASIGNACION      309
#define SUMA            310
#define RESTA           311
#define MULTIPLICACION  312
#define DIVISION        313
#define IGUAL           315
#define DISTINTO        316
#define MENOR           317
#define MENOR_IGUAL     318
#define MAYOR           319
#define MAYOR_IGUAL     320
#define AND_LOGICO      321
#define OR_LOGICO       322
#define NEGACION        323
#define SUMA_ASIGNACION 330
#define RESTA_ASIGNACION 331
#define MULT_ASIGNACION 332
#define DIV_ASIGNACION  333
#define INCREMENTO      340
#define DECREMENTO      341
#define DOS_PUNTOS      342
#define INTERROGACION   343

/* ============================================================
 * Codigos de literales (350-399)
 * ============================================================ */
#define NUMERO          350
#define NUMERO_DECIMAL  351
#define IDENTIFICADOR   352
#define CADENA          353

/* ============================================================
 * Tokens especiales (400+)
 * ============================================================ */
#define FIN_DE_FICHERO  400
#define ERROR_LEXICO    401

/* ============================================================
 * Variables globales de posicion (definidas en sistema_entrada.c)
 * ============================================================ */
extern int   lineaActual;
extern int   columnaActual;
extern FILE *archivoFuente;

/* ============================================================
 * Prototipos - Sistema de entrada (sistema_entrada.c)
 *
 * Implementa el doble buffer con centinela y los punteros
 * inicio/delantero para una lectura eficiente del fichero fuente.
 * ============================================================ */
int  abrirArchivoFuente(const char *nombreFichero);
void cerrarArchivoFuente(void);
int  siguienteCaracter(void);
void devolverCaracter(int c);
void moverInicio(void);

/* ============================================================
 * Prototipos - Tabla de simbolos (tabla_simbolos.c / TS.c)
 *
 * Tabla hash con direccionamiento abierto (sondeo lineal).
 * Almacena palabras reservadas (pre-cargadas) e identificadores.
 * ============================================================ */
void        inicializarTablaSimbolos(void);
void        liberarTablaSimbolos(void);
int         buscarSimbolo(const char *lexema);
int         insertarSimbolo(const char *lexema, int componente);
int         obtenerComponente(int indice);
const char *obtenerLexema(int indice);
void        imprimirTablaSimbolos(void);
void        imprimirPalabrasReservadas(void);
void        imprimirIdentificadores(void);

/* ============================================================
 * Prototipos - Analizador lexico (analizador.c)
 * ============================================================ */
int siguienteComponenteLexico(char *lexema);

/* ============================================================
 * Prototipos - Gestor de errores (errores.c)
 * ============================================================ */
void reportarErrorLexico(int linea, int columna, const char *mensaje);
void reportarAdvertencia(int linea, int columna, const char *mensaje);
int  obtenerCantidadErrores(void);
int  obtenerCantidadAdvertencias(void);
void reiniciarContadorErrores(void);
void imprimirResumenErrores(void);

#endif
