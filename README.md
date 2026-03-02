# Analizador Lexico para el Lenguaje D

Implementacion de un analizador lexico (scanner) para un subconjunto del lenguaje de programacion D. El analizador procesa un fichero fuente `.d` y descompone su contenido en componentes lexicos (tokens).

## Estructura del Proyecto

El proyecto esta organizado en modulos independientes:

| Archivo | Descripcion |
|---------|-------------|
| `definiciones.h` | Cabecera principal: definicion de tokens, estructuras de datos y prototipos de funciones |
| `sistema_entrada.c` | Sistema de entrada: lectura de caracteres del fichero fuente con lookahead de 1 caracter |
| `tabla_simbolos.c` | Tabla de simbolos: almacenamiento dinamico de palabras reservadas e identificadores |
| `analizador.c` | Analizador lexico: automata finito determinista modularizado en sub-automatas |
| `errores.c` | Gestor de errores: reporte centralizado de errores lexicos y advertencias |
| `main.c` | Programa principal: orquesta las tres fases (inicializacion, analisis, finalizacion) |
| `regression.d` | Fichero de prueba en lenguaje D con regresion lineal |
| `Makefile` | Fichero de compilacion |

## Compilacion

```bash
make
```

Para compilar y ejecutar directamente:

```bash
make run
```

Para limpiar archivos generados:

```bash
make clean
```

## Uso

```bash
./analizador                  # Analiza regression.d por defecto
./analizador fichero.d        # Analiza un fichero especifico
```

## Componentes Lexicos Soportados

### Palabras Reservadas
`import`, `while`, `double`, `int`, `void`, `foreach`, `cast`, `return`

### Operadores y Puntuacion
`;` `,` `.` `(` `)` `{` `}` `[` `]` `+` `-` `*` `/` `=` `==` `<` `++` `+=`

### Literales
- **Numeros enteros**: decimales (`123`), binarios (`0b1010`), hexadecimales (`0xFF`)
- **Numeros decimales**: con punto (`3.14`) y notacion cientifica (`1.0e+03`, `40e-1`)
- **Cadenas**: delimitadas por comillas dobles con soporte de secuencias de escape
- **Identificadores**: secuencias alfanumericas que comienzan con letra o `_`

### Comentarios (se descartan)
- Comentarios de linea: `//`
- Comentarios de bloque: `/* */`
- Comentarios anidados de D: `/+ +/` (con soporte de anidamiento)

## Arquitectura

### Flujo de Ejecucion

```
main (consumidor)  <-->  analizador lexico (productor)
       |                        |
       |  siguienteComponenteLexico()
       |<----- <componente, lexema> -----|
       |                        |
       |                  sistema de entrada
       |                        |
       |                  fichero fuente .d
```

1. El `main` actua como analizador sintactico simplificado (consumidor)
2. Llama repetidamente a `siguienteComponenteLexico()` (productor)
3. Recibe pares `<componente, lexema>` hasta `FIN_DE_FICHERO`

### Automata Finito Determinista

El analizador implementa un AFD modularizado en sub-automatas:

- **Estado inicial**: segun el primer caracter leido, se delega al sub-automata correspondiente
- **analizarNumero()**: reconoce enteros, binarios, hexadecimales, decimales y notacion cientifica
- **analizarIdentificador()**: reconoce identificadores y palabras reservadas (distinguidos por la tabla de simbolos)
- **analizarCadena()**: reconoce cadenas con secuencias de escape
- **saltarComentario*()**: consume y descarta comentarios de linea, bloque y anidados

### Tabla de Simbolos

- Usa **memoria dinamica** (malloc/realloc/free): array que crece automaticamente
- Se pre-cargan las palabras reservadas al inicializar
- Los identificadores se insertan durante el analisis (sin duplicados)
- Se libera toda la memoria al finalizar (sin punteros colgantes)

### Gestion de Errores

- Los errores lexicos se reportan con posicion (linea, columna)
- El analisis continua tras un error para detectar todos los errores posibles
- Al final se imprime un resumen con el total de errores y advertencias

## Salida del Programa

El programa produce:

1. **Palabras reservadas**: listado al inicializar la tabla de simbolos
2. **Componentes lexicos**: cada token encontrado con su posicion y tipo
3. **Tabla de simbolos**: todas las entradas (palabras reservadas + identificadores)
4. **Identificadores**: listado de identificadores encontrados
5. **Resumen de errores**: total de errores y advertencias
