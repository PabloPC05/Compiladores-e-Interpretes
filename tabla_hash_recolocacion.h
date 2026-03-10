#ifndef TABLA_HASH_RECOLOCACION_H
#define TABLA_HASH_RECOLOCACION_H

/*
 * tabla_hash_recolocacion.h
 *
 * Tabla hash generica con recolocacion (open addressing).
 * Adaptada para almacenar simbolos del lenguaje D:
 *   - clave:     lexema (cadena de hasta MAX_LONGITUD_ID chars)
 *   - dato:      componente (codigo numerico del token)
 *
 * Resolucion de colisiones:
 *   - tipoR=1: sondeo lineal      slot = (h + a*i) % N
 *   - tipoR=2: sondeo cuadratico  slot = (h + i^2) % N
 *
 * Funciones hash disponibles (tipoFH=1,2,3):
 *   1: suma de caracteres % N
 *   2/3: suma ponderada con factor K (polinomio de Horner) % N
 *
 * Factor de carga recomendado para recolocacion: L = n/N <= 0.5
 * Con TAM_HASH=509 y ~50 simbolos: L ~ 0.10 (muy bajo, excelente)
 *
 * Modificaciones aplicadas respecto a la version original:
 *   - TIPOELEMENTO adaptado a la tabla de simbolos (lexema + componente)
 *   - N definido como TAM_HASH (desde definiciones.h)
 *   - Eliminados campos nombre/alias/correo irrelevantes
 *   - Anadidas BuscarHashSlot() y ObtenerComponenteHash() para
 *     mantener compatibilidad con la API de tabla_simbolos.c
 */

#include "definiciones.h"

/* Tamano de la tabla: debe ser primo y >= 2*n (factor de carga <= 0.5)
 * Se usa TAM_HASH definido en definiciones.h                          */
#ifndef N
#define N TAM_HASH
#endif

/* Marcadores de celda */
#define VACIO   '\0'  /* celda nunca usada */
#define BORRADO ' '   /* celda borrada (lazy deletion) */

/* ============================================================
 * TIPOELEMENTO: entrada de la tabla de simbolos.
 * La clave es lexema[]; el dato es componente.
 * ============================================================ */
typedef struct {
    char lexema[MAX_LONGITUD_ID];  /* clave: nombre del simbolo */
    int  componente;               /* dato:  codigo del token   */
} TIPOELEMENTO;

/* TAD TablaHash: array estatico de N entradas */
typedef TIPOELEMENTO TablaHash[N];

/* ============================================================
 * Prototipos de la API generica
 * ============================================================ */

/** Inicializa todas las celdas a VACIO */
void InicializarTablaHash(TablaHash t);

/**
 * FuncionHash: calcula la posicion de cad en [0, N).
 * @tipoFH  1=suma simple, 2/3=polinomio de Horner con factor K
 * @K       factor de ponderacion (relevante para tipoFH=2/3)
 */
int FuncionHash(char *cad, unsigned int tipoFH, unsigned int K);

/**
 * EsMiembroHash: indica si clavebuscar esta en la tabla.
 * @return 1 si existe, 0 si no
 */
int EsMiembroHash(TablaHash t, char *clavebuscar,
                  unsigned int tipoFH, unsigned int K,
                  unsigned int tipoR,  unsigned int a);

/**
 * BuscarHash: busca clavebuscar y copia el elemento encontrado en *e.
 * @nPasosExtraB  acumula los pasos de sondeo realizados
 * @return 1 si encontrado, 0 si no
 */
int BuscarHash(TablaHash t, char *clavebuscar, TIPOELEMENTO *e,
               unsigned int tipoFH, unsigned int K,
               unsigned int tipoR,  unsigned int a,
               int *nPasosExtraB);

/**
 * BuscarHashSlot: busca clavebuscar y devuelve el indice del slot.
 * Funcion auxiliar para la tabla de simbolos (permite acceso
 * por indice compatible con buscarSimbolo/obtenerComponente).
 * @return indice del slot si existe, -1 si no
 */
int BuscarHashSlot(TablaHash t, char *clavebuscar,
                   unsigned int tipoFH, unsigned int K,
                   unsigned int tipoR,  unsigned int a);

/**
 * InsertarHash: inserta e en la tabla.
 * @nPasosExtraI  acumula los pasos de sondeo hasta encontrar hueco
 * @return 1 si hubo colision, 0 si no
 */
int InsertarHash(TablaHash t, TIPOELEMENTO e,
                 unsigned int tipoFH, unsigned int K,
                 unsigned int tipoR,  unsigned int a,
                 int *nPasosExtraI);

/**
 * BorrarHash: borra el elemento con clave claveborrar (lazy deletion).
 */
void BorrarHash(TablaHash t, char *claveborrar,
                unsigned int tipoFH, unsigned int K,
                unsigned int tipoR,  unsigned int a);

/**
 * ObtenerComponenteHash: devuelve el componente del slot dado.
 * @return codigo del token, o ERROR_LEXICO si el slot no es valido
 */
int ObtenerComponenteHash(TablaHash t, int slot);

#endif /* TABLA_HASH_RECOLOCACION_H */
