#ifndef DEFINICIONES_H
#define DEFINICIONES_H

/*
 * definiciones.h
 *
 * Archivo de cabecera principal del analizador lexico.
 * Contiene las definiciones de tokens, estructuras de datos
 * y prototipos de funciones utilizados por todos los modulos.
 *
 * Los codigos numericos de los tokens se dividen en rangos:
 *   - 273-299: Palabras reservadas del lenguaje D
 *   - 300-349: Operadores y signos de puntuacion
 *   - 350-399: Literales (numeros, cadenas, identificadores)
 *   - 400+:    Tokens especiales (EOF, errores)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Capacidad inicial de la tabla de simbolos (se redimensiona dinamicamente) */
#define CAPACIDAD_INICIAL 64

/* Longitud maxima de un lexema individual */
#define MAX_LONGITUD_LEXEMA 256

/* Longitud maxima de una linea de entrada */
#define MAX_LONGITUD_LINEA 1024

/* ============================================================
 * Codigos de palabras reservadas del lenguaje D
 * Cada palabra reservada tiene un codigo unico que permite
 * al analizador sintactico distinguirlas de los identificadores.
 * ============================================================ */
#define IMPORT  273
#define WHILE   274
#define DOUBLE  275
#define INT     276
#define VOID    277
#define FOREACH 278
#define CAST    279
#define RETURN  280

/* ============================================================
 * Codigos de operadores y signos de puntuacion
 * Para caracteres ASCII simples se podria usar su valor ASCII,
 * pero se asignan codigos propios para mayor claridad.
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
#define MENOR           317
#define SUMA_ASIGNACION 330
#define INCREMENTO      340

/* ============================================================
 * Codigos de literales
 * ============================================================ */
#define NUMERO          350
#define NUMERO_DECIMAL  351
#define IDENTIFICADOR   352
#define CADENA          353

/* ============================================================
 * Tokens especiales
 * ============================================================ */
#define FIN_DE_FICHERO  400
#define ERROR_LEXICO    401

/* ============================================================
 * Estructuras de datos para la tabla de simbolos
 *
 * Se usa memoria dinamica (malloc/realloc/free) para cumplir
 * con el requisito de no usar estructuras estaticas.
 * La tabla crece automaticamente cuando se llena.
 * ============================================================ */

/*
 * EntradaSimbolo: representa una entrada individual en la tabla.
 * - lexema: cadena dinamica con el texto del token
 * - componente: codigo numerico del tipo de token
 * - linea/columna: posicion en el fichero fuente (para depuracion)
 */
typedef struct {
    char *lexema;       /* Cadena dinamica (malloc) */
    int componente;     /* Codigo numerico del token */
    int linea;          /* Linea donde aparecio por primera vez */
    int columna;        /* Columna donde aparecio por primera vez */
} EntradaSimbolo;

/*
 * TablaSimbolos: estructura principal que almacena todas las entradas.
 * - entradas: array dinamico de EntradaSimbolo (crece con realloc)
 * - cantidad: numero actual de entradas
 * - capacidad: tamano actual del array (se duplica al llenarse)
 */
typedef struct {
    EntradaSimbolo *entradas;   /* Array dinamico (malloc/realloc) */
    int cantidad;               /* Numero de entradas actuales */
    int capacidad;              /* Capacidad actual del array */
} TablaSimbolos;

/* ============================================================
 * Variables globales
 * ============================================================ */
extern TablaSimbolos tablaSimbolos;
extern int lineaActual;
extern int columnaActual;
extern FILE *archivoFuente;

/* ============================================================
 * Prototipos - Sistema de entrada (sistema_entrada.c)
 *
 * El sistema de entrada abstrae la lectura del fichero fuente,
 * proporcionando caracteres uno a uno y manteniendo la posicion
 * (linea y columna) actualizada.
 * ============================================================ */
int  abrirArchivoFuente(const char *nombreFichero);
void cerrarArchivoFuente(void);
int  siguienteCaracter(void);
void devolverCaracter(int c);

/* ============================================================
 * Prototipos - Tabla de simbolos (tabla_simbolos.c)
 *
 * La tabla de simbolos almacena palabras reservadas (pre-cargadas
 * al inicio) e identificadores (insertados durante el analisis).
 * Permite distinguir entre ambos tipos de lexemas.
 * ============================================================ */
void inicializarTablaSimbolos(void);
void liberarTablaSimbolos(void);
int  buscarSimbolo(const char *lexema);
int  insertarSimbolo(const char *lexema, int componente, int linea, int columna);
int  obtenerComponente(int indice);
const char *obtenerLexema(int indice);
void imprimirTablaSimbolos(void);
void imprimirPalabrasReservadas(void);
void imprimirIdentificadores(void);

/* ============================================================
 * Prototipos - Analizador lexico (analizador.c)
 *
 * El analizador lexico es el productor: el analizador sintactico
 * (consumidor) llama a siguienteComponenteLexico() repetidamente
 * hasta recibir FIN_DE_FICHERO.
 * ============================================================ */
int siguienteComponenteLexico(char *lexema);

/* ============================================================
 * Prototipos - Gestor de errores (errores.c)
 *
 * Centraliza el manejo de errores lexicos y advertencias,
 * manteniendo contadores para el resumen final.
 * ============================================================ */
void reportarErrorLexico(int linea, int columna, const char *mensaje);
void reportarAdvertencia(int linea, int columna, const char *mensaje);
int  obtenerCantidadErrores(void);
int  obtenerCantidadAdvertencias(void);
void reiniciarContadorErrores(void);
void imprimirResumenErrores(void);

#endif
