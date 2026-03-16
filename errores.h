#ifndef ERRORES_H
#define ERRORES_H

// Mensajes de error 
// ERRORES FATALES
    // Fichero de entrada no existe o no se puede abrir
    #define ERR_FICHERO_NO_ABIERTO   "no se puede abrir el fichero"      
    // malloc devuelve NULL
    #define ERR_MEMORIA_INSUFICIENTE "memoria insuficiente"    
    // Tabla de simbolos llena tras redimensionar. Es un error muy muy improbable, pero lo comprobamos por seguridad         
    #define ERR_TS_LLENA             "tabla de simbolos llena"            

// ERRORES LEXICOS NO FATALES
    // Caracter no es operador, delimitador, letra, digito ni comilla
    #define ERR_CARACTER_NO_RECONOC  "caracter no reconocido"
    // EOF antes de cerrar /* */
    #define ERR_EOF_COMENTARIO_BLQ   "fin de fichero dentro de comentario '/*'"
    // EOF antes de cerrar /+ +/
    #define ERR_EOF_COMENTARIO_ANID  "fin de fichero dentro de comentario '/+'"
    // Identificador supera TAM_MAX_LEXEMA
    #define ERR_ID_DEMASIADO_LARGO   "identificador supera el limite de caracteres"
    // \x donde x no es n, t, r, \ ni "
    #define ERR_ESCAPE_DESCONOCIDO   "secuencia de escape desconocida"
    // 0x sin digitos hexadecimales
    #define ERR_HEX_SIN_DIGITOS      "literal hexadecimal sin digitos tras '0x'"
    // 0b sin digitos binarios
    #define ERR_BIN_SIN_DIGITOS      "literal binario sin digitos tras '0b'"
    // EOF antes de cerrar comilla de string
    #define ERR_STR_SIN_CERRAR       "cadena sin cerrar: fin de fichero inesperado"

// Error lexico no fatal se reporta con linea y columna para facilitar la localizacion del error en el codigo. 
// Se sigue ejecutando el código 
void report(int linea, int columna, const char *msg);

// Error fatal se reporta el error y se termina la ejecucion del programa. Se utiliza para errores graves como problemas de memoria o de apertura de fichero. 
void fatal(const char *msg);

#endif
