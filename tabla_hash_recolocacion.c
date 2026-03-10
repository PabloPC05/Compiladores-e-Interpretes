#include "tabla_hash_recolocacion.h"

/* ============================================================
 * TABLA HASH CON RECOLOCACION
 *
 * La clave de cada entrada es el campo lexema[].
 * VACIO ('\0') indica celda nunca usada.
 * BORRADO (' ') indica celda borrada (lazy deletion):
 *   - _PosicionBuscar salta los BORRADO (no para en ellos)
 *   - _PosicionInsertar puede reutilizar los BORRADO
 * ============================================================ */

void InicializarTablaHash(TablaHash t) {
    for (int i = 0; i < N; i++)
        t[i].lexema[0] = VACIO;
}

/* ============================================================
 * FUNCIONES HASH
 * ============================================================ */
int FuncionHash(char *cad, unsigned int tipoFH, unsigned int K) {
    int suma = 0, posicion = 0;
    switch (tipoFH) {
        case 1:
            /* Tipo 1: suma simple de caracteres */
            for (int i = strlen(cad) - 1; i >= 0; i--)
                suma += cad[i];
            posicion = suma % N;
            break;
        case 2:
        case 3:
            /* Tipo 2/3: polinomio de Horner con factor K
             * Recomendado para identificadores: K=31 o K=37 */
            for (int i = strlen(cad) - 1; i >= 0; i--)
                suma = (suma * K + cad[i]) % N;
            posicion = suma;
            break;
    }
    return posicion;
}

/* ============================================================
 * Funciones internas de posicionamiento
 * ============================================================ */

/*
 * _PosicionBuscar: localiza la posicion del elemento buscado.
 * Para al encontrar VACIO (no existe) o el lexema buscado (existe).
 * Salta sobre las celdas BORRADO (lazy deletion).
 */
static int _PosicionBuscar(TablaHash t, char *cad,
                            unsigned int tipoFH, unsigned int K,
                            unsigned int tipoR,  unsigned int a,
                            int *nPasosExtraB) {
    int posicion;
    int ini = FuncionHash(cad, tipoFH, K);

    for (int i = 0; i < N; i++) {
        switch (tipoR) {
            case 1: posicion = (ini + a * i) % N; break; /* lineal    */
            case 2: posicion = (ini + i * i) % N; break; /* cuadratica */
            default: posicion = (ini + i)     % N; break;
        }

        if (t[posicion].lexema[0] == VACIO) {
            /* Celda vacia: el elemento no existe */
            *nPasosExtraB += i;
            return posicion;
        }
        if (strcmp(t[posicion].lexema, cad) == 0) {
            /* Elemento encontrado */
            *nPasosExtraB += i;
            return posicion;
        }
        /* Celda BORRADO o colision: seguir sondeando */
    }
    return ini;
}

/*
 * _PosicionInsertar: localiza la primera celda libre (VACIO o BORRADO)
 * donde se puede insertar el elemento con clave cad.
 * Si el elemento ya existe, devuelve su posicion.
 */
static int _PosicionInsertar(TablaHash t, char *cad,
                              unsigned int tipoFH, unsigned int K,
                              unsigned int tipoR,  unsigned int a,
                              int *hayColisionI, int *nPasosExtraI) {
    int posicion;
    int ini = FuncionHash(cad, tipoFH, K);

    for (int i = 0; i < N; i++) {
        if (i > 0)
            *hayColisionI = 1;

        switch (tipoR) {
            case 1: posicion = (ini + a * i) % N; break;
            case 2: posicion = (ini + i * i) % N; break;
            default: posicion = (ini + i)     % N; break;
        }

        if (t[posicion].lexema[0] == VACIO ||
            t[posicion].lexema[0] == BORRADO) {
            /* Hueco encontrado */
            *nPasosExtraI += i;
            return posicion;
        }
        if (strcmp(t[posicion].lexema, cad) == 0) {
            /* Ya existe: devolver su posicion */
            *nPasosExtraI += i;
            return posicion;
        }
    }
    return ini;
}

/* ============================================================
 * API PUBLICA
 * ============================================================ */

int EsMiembroHash(TablaHash t, char *cad,
                  unsigned int tipoFH, unsigned int K,
                  unsigned int tipoR,  unsigned int a) {
    int dummy = 0;
    int posicion = _PosicionBuscar(t, cad, tipoFH, K, tipoR, a, &dummy);
    if (t[posicion].lexema[0] == VACIO)
        return 0;
    return (strcmp(t[posicion].lexema, cad) == 0);
}

int BuscarHash(TablaHash t, char *clavebuscar, TIPOELEMENTO *e,
               unsigned int tipoFH, unsigned int K,
               unsigned int tipoR,  unsigned int a,
               int *nPasosExtraB) {
    int posicion = _PosicionBuscar(t, clavebuscar, tipoFH, K, tipoR, a, nPasosExtraB);
    if (t[posicion].lexema[0] == VACIO)
        return 0;
    if (strcmp(t[posicion].lexema, clavebuscar) == 0) {
        *e = t[posicion];
        return 1;
    }
    return 0;
}

/*
 * BuscarHashSlot: igual que BuscarHash pero devuelve el indice del slot
 * en lugar de copiar el elemento. Usado por tabla_simbolos.c para
 * mantener la API basada en indices (buscarSimbolo/obtenerComponente).
 */
int BuscarHashSlot(TablaHash t, char *clavebuscar,
                   unsigned int tipoFH, unsigned int K,
                   unsigned int tipoR,  unsigned int a) {
    int dummy = 0;
    int posicion = _PosicionBuscar(t, clavebuscar, tipoFH, K, tipoR, a, &dummy);
    if (t[posicion].lexema[0] == VACIO)
        return -1;
    if (strcmp(t[posicion].lexema, clavebuscar) == 0)
        return posicion;
    return -1;
}

int InsertarHash(TablaHash t, TIPOELEMENTO e,
                 unsigned int tipoFH, unsigned int K,
                 unsigned int tipoR,  unsigned int a,
                 int *nPasosExtraI) {
    int hayColisionI = 0;
    int posicion = _PosicionInsertar(t, e.lexema, tipoFH, K, tipoR, a,
                                     &hayColisionI, nPasosExtraI);
    if (t[posicion].lexema[0] == VACIO ||
        t[posicion].lexema[0] == BORRADO) {
        t[posicion] = e;
    }
    return hayColisionI;
}

void BorrarHash(TablaHash t, char *cad,
                unsigned int tipoFH, unsigned int K,
                unsigned int tipoR,  unsigned int a) {
    int dummy = 0;
    int posicion = _PosicionBuscar(t, cad, tipoFH, K, tipoR, a, &dummy);
    if (t[posicion].lexema[0] != VACIO &&
        t[posicion].lexema[0] != BORRADO &&
        strcmp(t[posicion].lexema, cad) == 0) {
        t[posicion].lexema[0] = BORRADO;
        t[posicion].lexema[1] = VACIO;
    }
}

/*
 * ObtenerComponenteHash: devuelve el componente del slot indicado.
 * Permite a tabla_simbolos.c acceder al dato sin exponer la tabla.
 */
int ObtenerComponenteHash(TablaHash t, int slot) {
    if (slot >= 0 && slot < N &&
        t[slot].lexema[0] != VACIO &&
        t[slot].lexema[0] != BORRADO) {
        return t[slot].componente;
    }
    return ERROR_LEXICO;
}
