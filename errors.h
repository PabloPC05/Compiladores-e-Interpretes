#ifndef ERRORS_H
#define ERRORS_H

// Mensajes de error
// ERRORES FATALES
    // Fichero de entrada no existe o no se puede abrir
    #define ERR_FILE_NOT_OPENED    "no se puede abrir el fichero"
    // malloc devuelve NULL
    #define ERR_OUT_OF_MEMORY      "memoria insuficiente"
    // Tabla de simbolos llena tras redimensionar. Es un error muy muy improbable, pero lo comprobamos por seguridad
    #define ERR_TABLE_FULL         "tabla de simbolos llena"

// ERRORES LEXICOS NO FATALES
    // Caracter no es operador, delimitador, letra, digito ni comilla
    #define ERR_UNKNOWN_CHAR       "caracter no reconocido"
    // EOF antes de cerrar /* */
    #define ERR_EOF_BLOCK_COMMENT  "fin de fichero dentro de comentario '/*'"
    // EOF antes de cerrar /+ +/
    #define ERR_EOF_NESTED_COMMENT "fin de fichero dentro de comentario '/+'"
    // Identificador supera MAX_LEXEME_LEN
    #define ERR_ID_TOO_LONG        "identificador supera el limite de caracteres"
    // \x donde x no es n, t, r, \ ni "
    #define ERR_UNKNOWN_ESCAPE     "secuencia de escape desconocida"
    // 0x sin digitos hexadecimales
    #define ERR_HEX_NO_DIGITS      "literal hexadecimal sin digitos tras '0x'"
    // 0b sin digitos binarios
    #define ERR_BIN_NO_DIGITS      "literal binario sin digitos tras '0b'"
    // EOF antes de cerrar comilla de string
    #define ERR_UNTERMINATED_STR   "cadena sin cerrar: fin de fichero inesperado"

// Error lexico no fatal se reporta con linea y columna para facilitar la localizacion del error en el codigo.
// Se sigue ejecutando el código
void report(int line, int column, const char *msg);

// Error fatal se reporta el error y se termina la ejecucion del programa. Se utiliza para errores graves como problemas de memoria o de apertura de fichero.
void fatal(const char *msg);

#endif
