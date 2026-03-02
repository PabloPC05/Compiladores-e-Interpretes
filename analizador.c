/*
 * analizador.c
 *
 * Modulo principal del analizador lexico.
 *
 * Implementa un automata finito determinista (AFD) que reconoce
 * los componentes lexicos del lenguaje D presentes en el fichero fuente.
 *
 * El automata esta modularizado en sub-automatas (funciones) para
 * mejorar la legibilidad, facilitar la depuracion y permitir la
 * extension a nuevos tipos de lexemas:
 *
 *   - analizarNumero:       numeros enteros, binarios (0b), hex (0x),
 *                           decimales y con notacion cientifica (e/E)
 *   - analizarIdentificador: identificadores y palabras reservadas
 *   - analizarCadena:       cadenas delimitadas por comillas dobles
 *   - saltarComentarioLinea: comentarios //
 *   - saltarComentarioBloque: comentarios tradicionales de C
 *   - saltarComentarioAnidado: comentarios anidados de D (/+ +/)
 *
 * Flujo general:
 *   1. siguienteComponenteLexico() lee el siguiente caracter
 *   2. Segun el caracter leido, delega en el sub-automata adecuado
 *   3. El sub-automata consume caracteres hasta completar el lexema
 *   4. Si el sub-automata lee un caracter de mas, lo devuelve al buffer
 *   5. Retorna el codigo del componente lexico reconocido
 *
 * Cuando el AFD no puede reconocer un caracter o secuencia,
 * reporta un error lexico y continua con el siguiente caracter
 * para detectar todos los errores posibles en una sola pasada.
 */

#include "definiciones.h"

/*
 * verSiguiente: funcion auxiliar de lookahead.
 *
 * Lee el siguiente caracter del fichero sin consumirlo.
 * Internamente lee con siguienteCaracter() y lo devuelve
 * inmediatamente con devolverCaracter().
 *
 * Se usa para decidir transiciones del automata cuando
 * el caracter actual es ambiguo (por ejemplo, '/' puede
 * ser division, inicio de comentario de linea, de bloque
 * o de comentario anidado).
 *
 * Retorna: el siguiente caracter sin consumirlo.
 */
static int verSiguiente(void) {
    int c = siguienteCaracter();
    devolverCaracter(c);
    return c;
}

/* ============================================================
 * Sub-automatas para saltar comentarios
 *
 * Los comentarios no generan componentes lexicos; simplemente
 * se consumen y se descarta su contenido. El analizador lexico
 * continua buscando el siguiente token valido.
 * ============================================================ */

/*
 * saltarComentarioLinea: consume un comentario de linea (// ... \n).
 *
 * Lee caracteres hasta encontrar un salto de linea o EOF.
 * El salto de linea se devuelve al buffer porque no forma
 * parte del comentario (es un separador).
 *
 * Retorna: 0 siempre (los comentarios de linea no pueden fallar).
 */
static int saltarComentarioLinea(void) {
    int c;
    while ((c = siguienteCaracter()) != EOF && c != '\n')
        ;
    if (c == '\n')
        devolverCaracter(c);
    return 0;
}

/*
 * saltarComentarioBloque: consume un comentario de bloque estilo C.
 *
 * Lee caracteres hasta encontrar la secuencia de cierre.
 * Si llega a EOF sin cerrar el comentario, reporta un error.
 *
 * Retorna: 0 si el comentario se cerro correctamente, -1 si no.
 */
static int saltarComentarioBloque(void) {
    int lineaInicio = lineaActual;
    int c;
    while ((c = siguienteCaracter()) != EOF) {
        if (c == '*' && verSiguiente() == '/') {
            siguienteCaracter(); /* Consumir el '/' de cierre */
            return 0;
        }
    }
    reportarErrorLexico(lineaInicio, columnaActual, "Comentario de bloque sin cerrar");
    return -1;
}

/*
 * saltarComentarioAnidado: consume un comentario anidado de D (/+ ... +/).
 *
 * A diferencia de los comentarios de bloque de C, los comentarios
 * /+ +/ de D permiten anidamiento. Se mantiene un contador de
 * profundidad que se incrementa con cada /+ y se decrementa con
 * cada +/. El comentario termina cuando la profundidad llega a 0.
 *
 * Retorna: 0 si se cerro correctamente, -1 si llego a EOF sin cerrar.
 */
static int saltarComentarioAnidado(void) {
    int profundidad = 1;
    int lineaInicio = lineaActual;
    int c;

    while (profundidad > 0 && (c = siguienteCaracter()) != EOF) {
        if (c == '/' && verSiguiente() == '+') {
            siguienteCaracter(); /* Consumir el '+' */
            profundidad++;       /* Nuevo nivel de anidamiento */
        } else if (c == '+' && verSiguiente() == '/') {
            siguienteCaracter(); /* Consumir el '/' */
            profundidad--;       /* Cerrar un nivel de anidamiento */
        }
    }

    if (profundidad > 0) {
        reportarErrorLexico(lineaInicio, columnaActual, "Comentario anidado sin cerrar");
        return -1;
    }
    return 0;
}

/* ============================================================
 * Sub-automatas para reconocer lexemas complejos
 *
 * Cada funcion implementa un sub-automata del AFD principal.
 * Lee caracteres del sistema de entrada, construye el lexema
 * y devuelve el codigo del componente lexico reconocido.
 * ============================================================ */

/*
 * analizarNumero: reconoce literales numericos del lenguaje D.
 *
 * Tipos soportados:
 *   - Enteros decimales: 123, 1_000
 *   - Binarios: 0b1010, 0B110
 *   - Hexadecimales: 0xFF, 0X1A
 *   - Decimales con punto: 3.14, 6.5
 *   - Notacion cientifica: 1.0e+03, 40e-1
 *
 * Los separadores '_' son validos en D para legibilidad (8_000)
 * y se descartan del lexema final.
 *
 * Parametros:
 *   - c: primer digito ya leido
 *   - lexema: buffer donde se almacena el texto del numero
 *
 * Retorna: NUMERO o NUMERO_DECIMAL segun el tipo reconocido.
 */
static int analizarNumero(int c, char *lexema) {
    int i = 0;
    int esDecimal = 0;
    int tieneExponente = 0;

    lexema[i++] = c;

    /* Caso especial: literales binarios (0b/0B) y hexadecimales (0x/0X) */
    if (c == '0') {
        int sig = verSiguiente();

        /* Literal binario: 0b seguido de digitos binarios */
        if (sig == 'b' || sig == 'B') {
            lexema[i++] = siguienteCaracter();
            sig = verSiguiente();
            while (sig == '0' || sig == '1' || sig == '_') {
                int ch = siguienteCaracter();
                if (ch != '_') lexema[i++] = ch; /* Descartar separadores */
                sig = verSiguiente();
            }
            lexema[i] = '\0';
            return NUMERO;
        }

        /* Literal hexadecimal: 0x seguido de digitos hex */
        if (sig == 'x' || sig == 'X') {
            lexema[i++] = siguienteCaracter();
            sig = verSiguiente();
            while (isxdigit(sig) || sig == '_') {
                int ch = siguienteCaracter();
                if (ch != '_') lexema[i++] = ch; /* Descartar separadores */
                sig = verSiguiente();
            }
            lexema[i] = '\0';
            return NUMERO;
        }
    }

    /* Numero regular: digitos, separadores, parte decimal y exponente */
    while (1) {
        int sig = verSiguiente();

        if (isdigit(sig)) {
            /* Digito normal: agregar al lexema */
            lexema[i++] = siguienteCaracter();
        } else if (sig == '_') {
            /* Separador de legibilidad: consumir pero no agregar */
            siguienteCaracter();
        } else if (sig == '.' && !esDecimal && !tieneExponente) {
            /*
             * Punto decimal: solo si aun no hay parte decimal ni exponente.
             * Verificamos que despues del punto viene un digito para
             * distinguir "8." (entero seguido de punto) de "8.5" (decimal).
             */
            int punto = siguienteCaracter();
            if (isdigit(verSiguiente())) {
                esDecimal = 1;
                lexema[i++] = punto;
            } else {
                /* No es parte del numero: devolver el punto */
                devolverCaracter(punto);
                break;
            }
        } else if ((sig == 'e' || sig == 'E') && !tieneExponente) {
            /* Notacion cientifica: e/E seguido opcionalmente de +/- */
            esDecimal = 1;
            tieneExponente = 1;
            lexema[i++] = siguienteCaracter();
            sig = verSiguiente();
            if (sig == '+' || sig == '-') {
                lexema[i++] = siguienteCaracter();
            }
        } else {
            /* Caracter que no pertenece al numero: terminar */
            break;
        }
    }

    lexema[i] = '\0';
    return esDecimal ? NUMERO_DECIMAL : NUMERO;
}

/*
 * analizarIdentificador: reconoce identificadores y palabras reservadas.
 *
 * El automata acepta cadenas que comienzan con una letra o '_'
 * seguidas de letras, digitos o '_'. Los identificadores no pueden
 * empezar con un digito (eso lo maneja analizarNumero).
 *
 * Una vez reconocido el lexema, se busca en la tabla de simbolos:
 *   - Si existe como palabra reservada (pre-cargada), devuelve su codigo
 *   - Si no existe, se inserta como IDENTIFICADOR
 *
 * Parametros:
 *   - c: primer caracter ya leido (letra o '_')
 *   - lexema: buffer donde se almacena el texto del identificador
 *
 * Retorna: codigo de la palabra reservada o IDENTIFICADOR.
 */
static int analizarIdentificador(int c, char *lexema) {
    int i = 0;
    lexema[i++] = c;

    /* Leer mientras sea alfanumerico o guion bajo */
    int sig = verSiguiente();
    while (isalnum(sig) || sig == '_') {
        lexema[i++] = siguienteCaracter();
        sig = verSiguiente();
    }
    lexema[i] = '\0';

    /* Buscar en la tabla de simbolos (las keywords estan pre-cargadas) */
    int indice = buscarSimbolo(lexema);
    if (indice != -1) {
        return obtenerComponente(indice);
    }

    /* Identificador nuevo: insertar en la tabla */
    insertarSimbolo(lexema, IDENTIFICADOR, lineaActual, columnaActual);
    return IDENTIFICADOR;
}

/*
 * analizarCadena: reconoce cadenas de texto delimitadas por comillas dobles.
 *
 * Acepta secuencias de escape (\", \\, \n, etc.) dentro de la cadena.
 * Si llega a EOF sin encontrar la comilla de cierre, reporta error.
 *
 * Parametros:
 *   - lexema: buffer donde se almacena el texto de la cadena
 *             (incluyendo las comillas)
 *
 * Retorna: CADENA si se cerro correctamente, ERROR_LEXICO si no.
 */
static int analizarCadena(char *lexema) {
    int i = 0;
    int lineaInicio = lineaActual;
    lexema[i++] = '"';

    while (1) {
        int c = siguienteCaracter();

        if (c == EOF) {
            reportarErrorLexico(lineaInicio, columnaActual, "Cadena sin cerrar");
            lexema[i] = '\0';
            return ERROR_LEXICO;
        }

        lexema[i++] = c;

        /* Comilla de cierre: fin de la cadena */
        if (c == '"') {
            lexema[i] = '\0';
            return CADENA;
        }

        /* Secuencia de escape: consumir el siguiente caracter sin interpretarlo */
        if (c == '\\') {
            int siguiente = siguienteCaracter();
            if (siguiente != EOF) {
                lexema[i++] = siguiente;
            }
        }
    }
}

/* ============================================================
 * Funcion principal del analizador lexico.
 *
 * siguienteComponenteLexico() es la interfaz publica que el
 * analizador sintactico (consumidor) invoca repetidamente.
 * Cada llamada devuelve el siguiente componente lexico del
 * codigo fuente y almacena el lexema correspondiente.
 *
 * El bucle principal:
 *   1. Lee un caracter del sistema de entrada
 *   2. Salta espacios en blanco (separadores sin significado lexico)
 *   3. Segun el caracter, delega en el sub-automata apropiado:
 *      - '/' -> comentario o division
 *      - digito -> numero
 *      - letra/'_' -> identificador o palabra reservada
 *      - '"' -> cadena de texto
 *      - otros -> operadores y signos de puntuacion
 *   4. Si el caracter no corresponde a ningun patron, reporta error
 *
 * Retorna: codigo del componente lexico, o FIN_DE_FICHERO al
 *          llegar al final del fichero.
 * ============================================================ */
int siguienteComponenteLexico(char *lexema) {
    int c;

    while ((c = siguienteCaracter()) != EOF) {

        /* Saltar espacios en blanco (separadores sin significado lexico) */
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            continue;
        }

        /*
         * Comentarios y operador division.
         * El caracter '/' es ambiguo: puede iniciar un comentario
         * de linea (//), de bloque, anidado (/+), o ser el operador
         * de division. Se usa lookahead para distinguir.
         */
        if (c == '/') {
            int sig = verSiguiente();
            if (sig == '+') {
                siguienteCaracter(); /* Consumir '+' */
                if (saltarComentarioAnidado() < 0) return ERROR_LEXICO;
                continue; /* Comentario saltado, seguir buscando tokens */
            }
            if (sig == '/') {
                siguienteCaracter(); /* Consumir segundo '/' */
                saltarComentarioLinea();
                continue;
            }
            if (sig == '*') {
                siguienteCaracter(); /* Consumir '*' */
                if (saltarComentarioBloque() < 0) return ERROR_LEXICO;
                continue;
            }
            /* No es comentario: es el operador de division */
            lexema[0] = '/';
            lexema[1] = '\0';
            return DIVISION;
        }

        /* Numeros: delegar al sub-automata de numeros */
        if (isdigit(c)) {
            return analizarNumero(c, lexema);
        }

        /*
         * Identificadores y palabras reservadas.
         * isalpha() verifica si el caracter esta en el rango alfabetico ASCII.
         * Los identificadores en D pueden empezar con '_' o letra,
         * pero nunca con un digito.
         */
        if (isalpha(c) || c == '_') {
            return analizarIdentificador(c, lexema);
        }

        /* Cadenas de texto: delegar al sub-automata de cadenas */
        if (c == '"') {
            return analizarCadena(lexema);
        }

        /*
         * Operadores y signos de puntuacion.
         * Para operadores de un solo caracter, se devuelve directamente.
         * Para operadores compuestos (++, +=, ==), se usa lookahead
         * para verificar si el siguiente caracter forma un operador
         * de dos caracteres.
         */
        lexema[0] = c;

        switch (c) {
            case ';': lexema[1] = '\0'; return PUNTO_Y_COMA;
            case '(': lexema[1] = '\0'; return PARENTESIS_IZQ;
            case ')': lexema[1] = '\0'; return PARENTESIS_DER;
            case '{': lexema[1] = '\0'; return LLAVE_IZQ;
            case '}': lexema[1] = '\0'; return LLAVE_DER;
            case '[': lexema[1] = '\0'; return CORCHETE_IZQ;
            case ']': lexema[1] = '\0'; return CORCHETE_DER;
            case ',': lexema[1] = '\0'; return COMA;
            case '.': lexema[1] = '\0'; return PUNTO;
            case '-': lexema[1] = '\0'; return RESTA;
            case '*': lexema[1] = '\0'; return MULTIPLICACION;

            /* Operador '+': puede ser suma, incremento (++) o suma-asignacion (+=) */
            case '+':
                if (verSiguiente() == '+') {
                    lexema[1] = siguienteCaracter();
                    lexema[2] = '\0';
                    return INCREMENTO;
                }
                if (verSiguiente() == '=') {
                    lexema[1] = siguienteCaracter();
                    lexema[2] = '\0';
                    return SUMA_ASIGNACION;
                }
                lexema[1] = '\0';
                return SUMA;

            /* Operador '=': puede ser asignacion o comparacion de igualdad (==) */
            case '=':
                if (verSiguiente() == '=') {
                    lexema[1] = siguienteCaracter();
                    lexema[2] = '\0';
                    return IGUAL;
                }
                lexema[1] = '\0';
                return ASIGNACION;

            /* Operador '<': comparacion menor que */
            case '<':
                lexema[1] = '\0';
                return MENOR;

            /* Caracter no reconocido: error lexico */
            default:
                lexema[1] = '\0';
                reportarErrorLexico(lineaActual, columnaActual,
                                    "Caracter no reconocido");
                return ERROR_LEXICO;
        }
    }

    /* Se alcanzo el final del fichero: devolver token EOF */
    strcpy(lexema, "EOF");
    return FIN_DE_FICHERO;
}
