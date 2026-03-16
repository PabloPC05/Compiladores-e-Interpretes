#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"
#include "definitions.h"
#include "errors.h"

#define INITIAL_SIZE     101
#define MAX_LOAD_FACTOR  70

// Copia un lexema a memoria dinámica
static char *copyLexeme(const char *src) {
    char *copy = malloc(strlen(src) + 1);
    if (copy) strcpy(copy, src);
    return copy;
}

// Funcion hash FNV-1a: h = (h ^ c) * prime. Devuelve el hash crudo (sin modulo)
static unsigned int hashFnv1a(const char *str) {
    unsigned int h = 2166136261u; // Valor inicial estándar para FNV-1a
    while (*str) {
        // Mezclamos el caracter actual en el hash y luego multiplicamos por el primo
        h ^= (unsigned char)*str++;
        h *= 16777619u;
    }
    return h;
}

typedef struct {
    Symbol *table;
    int     size;
    int     count;
} SymbolTable;

static SymbolTable symbolTable = { .table = NULL, .size = 0, .count = 0 };

// Declaraciones adelantadas
static Symbol* createTable(int size);
static void addKeywords();

// Redimensiona la tabla al doble (+1) de su tamaño actual
static void resizeTable() {
    int newSize = symbolTable.size * 2 + 1;
    Symbol *newTable = createTable(newSize);

    // Reinsertar cada entrada en la nueva tabla recalculando posiciones
    for (int i = 0; i < symbolTable.size; i++) {
        if (symbolTable.table[i].lexeme == NULL) continue;

        // Recalcular la posición del lexema en la nueva tabla usando el nuevo tamaño
        unsigned int pos = hashFnv1a(symbolTable.table[i].lexeme) % newSize;
        // Si hay colisión, buscar la siguiente posición disponible (sondeo lineal)
        while (newTable[pos].lexeme != NULL)
            pos = (pos + 1) % newSize;

        // Mover el puntero al lexema, no copiarlo
        newTable[pos].lexeme = symbolTable.table[i].lexeme;
        newTable[pos].token  = symbolTable.table[i].token;
    }

    // Liberar el array viejo (no los lexemas) y actualizar
    free(symbolTable.table);
    symbolTable.table = newTable;
    symbolTable.size  = newSize;
}

static Symbol* createTable(int size){
    Symbol *table = malloc(size * sizeof(Symbol));
    if (!table) fatal(ERR_OUT_OF_MEMORY);

    for (int i = 0; i < size; i++) {
        table[i].lexeme = NULL;
        table[i].token  = TOKEN_INVALID;
    }

    return table;
}

void initSymbolTable() {
    symbolTable.table = createTable(INITIAL_SIZE);
    symbolTable.size  = INITIAL_SIZE;
    symbolTable.count = 0;
    addKeywords();
}

// Palabras reservadas (8) presentes en regression.d
static void addKeywords(){
    lookupOrInsert("import",   KW_IMPORT);
    lookupOrInsert("while",    KW_WHILE);
    lookupOrInsert("double",   KW_DOUBLE);
    lookupOrInsert("int",      KW_INT);
    lookupOrInsert("void",     KW_VOID);
    lookupOrInsert("foreach",  KW_FOREACH);
    lookupOrInsert("cast",     KW_CAST);
    lookupOrInsert("return",   KW_RETURN);
}


static int findSlot(const char *lexeme, unsigned int h) {
    unsigned int pos = h % symbolTable.size;

    for (int i = 0; i < symbolTable.size; i++) {
        unsigned int p = (pos + i) % symbolTable.size;

        // Si encontramos una posición vacía, el lexema no está en la tabla y esa es la posición donde se debería insertar
        if (symbolTable.table[p].lexeme == NULL)
            return p;

        // Si encontramos el lexema, devolvemos su posición
        if (strcmp(symbolTable.table[p].lexeme, lexeme) == 0)
            return p;
    }

    return -1; // tabla llena
}


Symbol lookupOrInsert(const char *lexeme, int newToken) {
    // Si el factor de carga supera el máximo permitido, redimensionamos la tabla antes de insertar
    if (symbolTable.count * 100 / symbolTable.size > MAX_LOAD_FACTOR) resizeTable();

    // Calculamos el hash del lexema y buscamos la posición donde debería estar o donde debería ser insertado
    unsigned int h = hashFnv1a(lexeme);
    int pos = findSlot(lexeme, h);

    // Si la tabla está llena (no debería ocurrir tras redimensionar, pero lo comprobamos por seguridad), reportamos error fatal
    if (pos == -1) {
        fatal(ERR_TABLE_FULL);
        return makeSymbol(TOKEN_INVALID, NULL);
    }

    // Si el lexema ya existe en la tabla, devolvemos su componente léxico
    if (symbolTable.table[pos].lexeme != NULL)
        return symbolTable.table[pos];

    // Si el lexema no existe, lo insertamos con el token dado
    symbolTable.table[pos].lexeme = copyLexeme(lexeme);

    // Si no se pudo copiar el lexema por falta de memoria, reportamos error fatal
    if (!symbolTable.table[pos].lexeme) {
        fatal(ERR_OUT_OF_MEMORY);
    }

    // Insertamos el nuevo token y actualizamos el número de entradas
    symbolTable.table[pos].token = newToken;
    symbolTable.count++;

    return symbolTable.table[pos];
}


void printSymbolTable() {
    printf("=== Tabla de simbolos ===\n");
    int n = 0;
    for (int i = 0; i < symbolTable.size; i++) {
        if (symbolTable.table[i].lexeme != NULL){
            n++;
            printf("  [%3d] token=%-4d  lexeme=%s\n", i, symbolTable.table[i].token, symbolTable.table[i].lexeme);
        }
    }
    printf("Total de entradas: %d\n", n);
    printf("=========================\n\n");
}


void freeSymbolTable() {
    for (int i = 0; i < symbolTable.size; i++) {
        if (symbolTable.table[i].lexeme != NULL) {
            free(symbolTable.table[i].lexeme);
            symbolTable.table[i].lexeme = NULL;
        }
    }
    free(symbolTable.table);
    symbolTable.table = NULL;
    symbolTable.size  = 0;
    symbolTable.count = 0;
}
