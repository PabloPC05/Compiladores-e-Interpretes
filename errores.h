#ifndef ERRORES_H
#define ERRORES_H

#define ERR_FICHERO_NO_ABIERTO   "no se puede abrir el fichero"
#define ERR_MEMORIA_INSUFICIENTE "memoria insuficiente"
#define ERR_TS_LLENA             "tabla de simbolos llena"
#define ERR_CARACTER_NO_RECONOC  "caracter no reconocido"
#define ERR_EOF_COMENTARIO_BLQ   "fin de fichero dentro de comentario '/*'"
#define ERR_EOF_COMENTARIO_ANID  "fin de fichero dentro de comentario '/+'"
#define ERR_ID_DEMASIADO_LARGO   "identificador supera el limite de caracteres"
#define ERR_ESCAPE_DESCONOCIDO   "secuencia de escape desconocida"

void report(const char *msg, int linea, int columna, int fatal);

#endif
