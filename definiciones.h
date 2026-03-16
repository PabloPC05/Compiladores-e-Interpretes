#ifndef DEFINICIONES_H
#define DEFINICIONES_H

/* Tamaño de cada mitad del doble buffer */
#define TAM_BUFFER 1024

/* Longitud maxima de un lexema */
#define TAM_MAX_LEXEMA 512

/* Tokens especiales */
#define TOKEN_INVALIDO  -1
#define TOKEN_EOF       256
#define TOKEN_ERROR     -2

/* Literales */
#define LIT_ENTERO      257
#define LIT_FLOTANTE    258
#define LIT_STRING      259

/* Identificador */
#define IDENTIFICADOR   260

/* Palabras reservadas (a partir de 273 segun el enunciado) */
#define KW_IMPORT       273
#define KW_WHILE        274
#define KW_DOUBLE       275
#define KW_INT          276
#define KW_VOID         277
#define KW_FOREACH      278
#define KW_CAST         279
#define KW_ENFORCE      280
#define KW_RETURN       281
#define KW_IF           282
#define KW_ELSE         283
#define KW_FOR          284
#define KW_DO           285
#define KW_BREAK        286
#define KW_CONTINUE     287
#define KW_SWITCH       288
#define KW_CASE         289
#define KW_DEFAULT      290

/* Operadores de dos caracteres */
#define OP_EQ           291   /* == */
#define OP_NEQ          292   /* != */
#define OP_LEQ          293   /* <= */
#define OP_GEQ          294   /* >= */
#define OP_AND          295   /* && */
#define OP_OR           296   /* || */
#define OP_SHL          297   /* << */
#define OP_SHR          298   /* >> */
#define OP_PLUS_ASSIGN  299   /* += */
#define OP_MINUS_ASSIGN 300   /* -= */
#define OP_MUL_ASSIGN   301   /* *= */
#define OP_DIV_ASSIGN   302   /* /= */
#define OP_MOD_ASSIGN   303   /* %= */
#define OP_INC          304   /* ++ */
#define OP_DEC          305   /* -- */

/* Componente lexico: token + lexema (memoria dinamica) */
typedef struct {
    int   token;
    char *lexema;
} ComponenteLexico;

static inline ComponenteLexico make_cl(int token, char *lexema){
    ComponenteLexico cl;
    cl.token  = token;
    cl.lexema = lexema;
    return cl;
}

#endif /* DEFINICIONES_H */
