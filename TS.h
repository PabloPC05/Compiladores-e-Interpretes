#ifndef TS_H
#define TS_H

#include "definiciones.h"

void inicializar_TS(void);
/* int buscar_TS(const char *lexema); */
void insertar_TS(const char *lexema, int token);
int buscar_o_insertar_TS(const char *lexema, int token_nuevo);
void imprimir_TS(void);
void liberar_TS(void);

#endif /* TS_H */
