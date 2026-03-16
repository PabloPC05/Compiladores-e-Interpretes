Analizador lexico del lenguaje D
Practica 1 - Compiladores e Interpretes

Compilado con: gcc (std=c99, flags -Wall -Wextra)
Probado en: Ubuntu 22.04 LTS


Instrucciones
=======================================

  Compilar:             make
  Ejecutar:             make run       (analiza regression.d)
  Borrar ejecutable:    make clean


Estructura de ficheros
=======================================

  definiciones.h          Codigos de token, constantes (TAM_BUFFER, TAM_MAX_LEXEMA) y el struct ComponenteLexico {token, lexema}.
  SI.c / SI.h             Sistema de entrada con doble buffer y centinela.
  TS.c / TS.h             Tabla de simbolos (hash abierto con sondeo lineal).
  AL.c / AL.h             Analizador lexico: automata que produce componentes lexicos.
  AS.c / AS.h             Analizador sintactico: consume componentes e imprime.
  errores.c / errores.h   Gestion centralizada de errores (report y fatal).
  main.c                  Punto de entrada: inicializa, ejecuta e imprime la tabla.


Decisiones de diseno
=======================================

Sistema de entrada (SI)

- Doble buffer de TAM_BUFFER (1024) caracteres cada mitad, con un caracter
  centinela ('\0') al final de cada bloque. La carga se hace con fread() de
  un bloque completo cada vez que se necesita, no caracter a caracter.
- Dos punteros, inicio y delantero, recorren los buffers. El lexema se
  extrae copiando desde inicio hasta delantero, cruzando la frontera entre
  bloques si es necesario.
- El centinela se distingue del EOF real comprobando feof(): si el fichero
  no ha terminado, el '\0' es centinela y se carga el siguiente bloque;
  si feof() es verdadero, es fin real del fichero.
- Si un lexema ocupa mas de un bloque, get_lexema() concatena las dos
  partes. La longitud maxima de un lexema esta limitada a TAM_MAX_LEXEMA
  (512 caracteres); superarla genera un aviso pero el analisis continua.

Tabla de simbolos (TS)

- Tabla hash con direccionamiento abierto (sondeo lineal) y funcion
  hash FNV-1a. Tamano inicial 101 entradas.
- Cuando el factor de carga supera el 70%, la tabla se redimensiona al
  doble + 1, reinsertando todas las entradas. Para regression.d no llega
  a redimensionar, pero el mecanismo esta implementado por generalidad.
- Las palabras reservadas se insertan al inicializar la tabla, antes del
  analisis. Asi, al buscar un identificador, si ya existe como palabra
  reservada se devuelve su token correspondiente.
- Los lexemas se almacenan en memoria dinamica (malloc/free). Al liberar
  la tabla se liberan todos los lexemas y se apuntan a NULL.

Analizador lexico (AL) -----------------------

- Un unico automata implementado con funciones modulares: una por cada
  tipo de lexema (identificador, numero, string, operador/delimitador).
- Reconoce los tres tipos de comentario de D:
    // (linea)       se consume hasta '\n' o EOF.
    /* */ (bloque)   se consume hasta encontrar "*/".
    /+ +/ (anidado)  se usa un contador de profundidad (no una pila),
                     ya que solo necesitamos la profundidad actual.
- Literales numericos soportados: enteros decimales, binarios (0b/0B),
  hexadecimales (0x/0X), flotantes con parte decimal y/o exponente
  (e/E con signo opcional). Se aceptan guiones bajos como separadores
  dentro de los numeros, tal como hace D.
- Los operadores de un solo caracter se devuelven con su valor ASCII como
  codigo de token. Los de dos caracteres (++, +=, ==, etc.) tienen
  codigos propios definidos en definiciones.h.
- Para resolver ambiguedades entre operadores de uno y dos caracteres
  (por ejemplo '<' vs '<=' vs '<<'), se usa una funcion match() que
  intenta consumir el segundo caracter y, si no coincide, lo devuelve.

Analizador sintactico (AS) --------------------------

- Actua como consumidor en el patron productor-consumidor: llama
  repetidamente a sig_comp_lexico() e imprime cada componente lexico
  con su token y lexema hasta recibir TOKEN_EOF.
- El main solo interactua con el analizador sintactico y con la tabla
  de simbolos (para inicializarla e imprimirla al inicio y al final).

Tratamiento de errores ----------------------

- Todos los mensajes de error estan definidos como constantes en errores.h.
  Se imprimen a traves de dos funciones centralizadas:
    report(linea, columna, mensaje)  para errores lexicos no fatales.
    fatal(mensaje)                   para errores que terminan el programa.
- Errores fatales (exit inmediato): fichero no encontrado, memoria
  insuficiente, tabla de simbolos llena.
- Errores no fatales (se imprime aviso y se continua): caracter no
  reconocido, comentario sin cerrar, string sin cerrar, identificador
  demasiado largo, secuencia de escape desconocida, literal hex/bin
  sin digitos tras el prefijo.

Gestion de memoria ------------------

- No se almacena el codigo fuente completo en memoria; el sistema de
  entrada lee bloques bajo demanda.
- Cada lexema devuelto por el analizador lexico se aloja con malloc y
  se libera tras ser procesado por el analizador sintactico.
- Al finalizar, se libera la tabla de simbolos (todos los lexemas y el
  array), se cierra el fichero y se anulan los punteros.
