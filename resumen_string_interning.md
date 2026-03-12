# Resumen completo: Practica 1 - Analizador Lexico para el lenguaje D

## Descripcion general

Practica de la asignatura Compiladores e Interpretes (UCM). Consiste en implementar un **analizador lexico** para un subconjunto del lenguaje de programacion D, analizando el archivo `regression.d`. Vale 2 puntos sobre 5 de laboratorios.

---

## Arquitectura del proyecto

El proyecto sigue un patron **productor-consumidor** con modulos independientes:

```
main.c
  |-> TS (Tabla de Simbolos)     : almacena keywords e identificadores unicos
  |-> AS (Analizador Sintactico) : pide tokens al AL e imprime resultados
       |-> AL (Analizador Lexico)  : reconoce tokens mediante automatas
            |-> SI (Sistema de Entrada) : doble buffer circular para leer caracteres
```

### Archivos del proyecto

| Archivo | Responsabilidad |
|---------|----------------|
| `definiciones.h` | Tokens (macros), struct `ComponenteLexico` |
| `errores.h / errores.c` | Constantes de error como strings, funcion `report()` |
| `SI.h / SI.c` | Sistema de entrada con doble buffer (centinela EOF) |
| `AL.h / AL.c` | Analizador lexico: automatas para identificadores, numeros, strings, operadores |
| `TS.h / TS.c` | Tabla de simbolos: hash DJB2 + linear probing |
| `AS.h / AS.c` | Analizador sintactico: bucle que consume tokens e imprime |
| `main.c` | Inicializacion, ejecucion y limpieza |

---

## Flujo de ejecucion

```
1. main: inicializar_TS()       -> crea tabla hash, inserta 18 palabras reservadas
2. main: imprimir_TS()          -> muestra keywords (verificacion)
3. main: inicializar_AS(file)   -> abre fichero via SI
4. main: analizar()             -> bucle principal:
   4.1 AS: sig_comp_lexico()    -> AL lee caracteres del SI
       - Salta espacios y comentarios (//, /* */, /+ +/)
       - Mueve puntero inicio
       - Lee primer caracter y decide automata:
         * [a-zA-Z_] -> leer_identificador -> buscar_o_insertar_TS -> CL interned
         * [0-9]     -> leer_numero (dec/hex/bin/float) -> CL con malloc
         * '"'       -> leer_string -> CL con malloc
         * otro      -> procesar_operador_o_delimitador -> CL con malloc
   4.2 AS: imprime <token, lexema>
   4.3 AS: free(cl.lexema) solo si NO es interned (literales/operadores)
   4.4 Repite hasta TOKEN_EOF
5. main: imprimir_TS()          -> muestra keywords + identificadores encontrados
6. main: liberar_TS()           -> free de todos los lexemas de la tabla
7. main: cerrar_AS()            -> cierra fichero
```

---

## Modulos en detalle

### Sistema de Entrada (SI.c)

- **Doble buffer** de TAM_BUFFER (1024) caracteres cada uno, con centinela EOF al final
- Variables encapsuladas en `static EstadoSI si` (no hay variables globales expuestas)
- Funciones: `sig_caracter()`, `devolver_caracter()`, `get_lexema()`, `mover_inicio()`
- `get_lexema()` hace `malloc` + `memcpy` del contenido entre `inicio` y `delantero`
- Maneja el cruce de bloques (buffer circular)

### Analizador Lexico (AL.c)

- Variables de posicion `linea_actual` y `columna_actual` son `static` (encapsuladas)
- Funciones auxiliares marcadas `static inline`: `leer_char`, `devolver_char`, `make_cl`, `match`, `consumir_hex/bin/dec`, `leer_parte_decimal`, `leer_exponente`
- Comentarios: linea `//`, bloque `/* */`, anidado `/+ +/` (con contador de profundidad)
- Numeros: decimales, hexadecimales (0x), binarios (0b), flotantes (parte decimal + exponente)
- Strings: delimitados por comillas dobles, con secuencias de escape
- Operadores: switch con match() para operadores de 1 y 2 caracteres

### Tabla de Simbolos (TS.c)

- Hash table de tamano 101 con hash DJB2 y linear probing
- Usa `ComponenteLexico` directamente como tipo de entrada (no hay struct separado)
- Una sola funcion publica: `buscar_o_insertar_TS(lexema, token)` que devuelve `ComponenteLexico`
- **String interning**: los identificadores/keywords comparten el puntero al lexema de la TS
- `copiar_lexema()` usa malloc+strcpy para crear la copia canonica en la TS

### Analizador Sintactico (AS.c)

- Bucle do-while que llama a `sig_comp_lexico()` e imprime cada CL
- Libera lexemas solo de tokens que NO son interned (literales, operadores)
- Cuenta componentes lexicos totales

### Errores (errores.h / errores.c)

- Constantes de error definidas como strings (`#define ERR_... "mensaje"`)
- Funcion `report(msg, linea, columna, fatal)`: imprime en stderr, si fatal hace exit(1)
- Errores fatales: fichero no abierto, memoria insuficiente
- Errores no fatales: TS llena, caracter no reconocido, EOF en comentario, escape desconocido, ID demasiado largo

---

## String interning (implementado)

Los identificadores y palabras reservadas comparten el puntero al lexema almacenado en la TS. Los literales y operadores tienen su propio malloc que se libera en el AS.

```
Identificador "x" aparece 3 veces:
  TS: tabla[pos].lexema = 0xA000 -> "x"   (unica copia)
  CL1.lexema = 0xA000 --^                  (interned)
  CL2.lexema = 0xA000 --^                  (interned)
  CL3.lexema = 0xA000 --^                  (interned)

Literal "42" aparece 2 veces:
  CL1.lexema = 0xB000 -> "42"   (malloc propio, se libera en AS)
  CL2.lexema = 0xC000 -> "42"   (malloc propio, se libera en AS)
```

En el AS se distingue con: `if (cl.token != IDENTIFICADOR && cl.token < KW_IMPORT) free(cl.lexema);`

---

## Tokens soportados

- **Palabras reservadas** (273-290): import, while, double, int, void, foreach, cast, enforce, return, if, else, for, do, break, continue, switch, case, default
- **Literales**: LIT_ENTERO (257), LIT_FLOTANTE (258), LIT_STRING (259)
- **Identificador**: IDENTIFICADOR (260)
- **Operadores compuestos** (291-305): ==, !=, <=, >=, &&, ||, <<, >>, +=, -=, *=, /=, %=, ++, --
- **Operadores/delimitadores simples**: se representan por su valor ASCII (<256)
- **Especiales**: TOKEN_EOF (256), TOKEN_INVALIDO (-1), TOKEN_ERROR (-2)

---

## Resultados de pruebas (antes de los ultimos cambios)

- Compila con `gcc -Wall -Wextra -std=c99` con **0 warnings**
- Analiza regression.d produciendo ~250 componentes lexicos
- Un error esperado: comentario `/*` sin cerrar en linea 53 de regression.d
- Valgrind: **0 errores, 0 memory leaks** (pendiente de re-verificar tras string interning)

---

## Bugs corregidos durante el desarrollo

1. **`leer_parte_decimal`**: Tras comprobar `c == '.'`, testeaba `isdigit(c)` pero `c` seguia siendo '.'. Solucion: leer siguiente char tras confirmar el punto, y backtrack de 2 chars si falla.

2. **`leer_exponente`**: Consumia 2 chars cuando solo necesitaba 1 para el signo. Backtrack asimetrico. Solucion: contador `devueltos` para backtrack exacto.

3. **`saltar_espacios_y_comentarios`**: Al leer un caracter que no era espacio ni '/', salia del bucle sin devolver el caracter. Esto causaba que se perdiera el primer caracter de cada token. Solucion: anadir `devolver_char()`.

4. **`consumir_*/leer_identificador`**: Leian un caracter de mas sin devolverlo. Antes estaba enmascarado por el bug anterior (se cancelaban mutuamente). Solucion: anadir `devolver_char()` al final de cada una.

5. **Bucle infinito en EOF**: Tras corregir `saltar_espacios_y_comentarios`, EOF se devolvía y re-leia infinitamente. Solucion: `if (c != (char)EOF) devolver_char()`.

6. **Doble impresion de TS**: `inicializar_TS()` tenia su propio `imprimir_TS()` ademas del que hace main. Pendiente de eliminar.

---

## Tareas pendientes (de apuntes_lab4.md)

- [ ] Tratar "0bx" (binario invalido): devolver '0' como LIT_ENTERO y "bx" como identificador
- [ ] Valorar AFD en lugar de contador para comentarios anidados /+ +/
- [ ] Revisar cadenas de if-else en AL.c y sustituir por switch donde sea posible
- [ ] Evaluar si se puede eliminar string.h y ctype.h con implementaciones nativas
- [ ] Eliminar prints duplicados de inicializar_TS (lineas 59-61 de TS.c)
- [ ] Re-verificar con valgrind tras los cambios de string interning
- [ ] Limpiar codigo muerto/comentado

---

## Decisiones de diseno y feedback del usuario

- Solo reportar **errores**, no warnings/avisos
- Errores como **constantes string**, no codigos numericos
- `report()` recibe solo mensaje + linea + columna + fatal (sin campo "extra")
- No usar `peek()`: el consumo de caracteres debe ser lineal
- Uso de `static inline` en funciones auxiliares internas
- Variables globales encapsuladas con `static` dentro de cada modulo (struct `EstadoSI` en SI.c)
- C no soporta sobrecarga de funciones (se pregunto, se descarto)
- Se compila y prueba en WSL (Linux) aunque se desarrolla en Windows
- `ComponenteLexico` y `EntradaTS` se unificaron en un solo tipo
- `insertar_TS` se elimino por ser identica a `buscar_o_insertar_TS`
- Se elimino el campo `hash` de la entrada de la TS por simplicidad

---

## Requisitos del profesor (de los apuntes de laboratorio)

- Doble buffer con centinela EOF (no se puede almacenar el codigo fuente en memoria)
- Tabla hash propia (no paquetes externos)
- `main.c` tiene 3 fases: inicializacion, compilacion, limpieza
- Imprimir TS al inicio (keywords) y al final (keywords + identificadores)
- Valgrind sin leaks ni errores
- Sin warnings de compilacion
- Codigo muy bien comentado y documentado
- Entrega como .zip, sin nombre del alumno en la carpeta
- Debe funcionar en Linux (Ubuntu)
- Usar el minimo de copias del lexema posible (eficiencia)
- El manejo erroneo de memoria dinamica es un error grande
