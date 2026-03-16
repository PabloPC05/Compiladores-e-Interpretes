Analizador Lexico del Lenguaje D
Practica 1 - Compiladores e Interpretes

Compilado con: gcc 13.3.0 (C99, flags -Wall -Wextra)
Sistema: Ubuntu 24.04.3 LTS (WSL)

COMO USARLO
=======================================
  - Compilar:          make
  - Ejecutar:          make run (procesa regression.d)
  - Limpiar archivos:  make clean


ARCHIVOS PRINCIPALES
=======================================
  - input_system.c / .h:      Sistema de Entrada. Lee el codigo fuente por bloques.
  - lexical_analyzer.c / .h:  Analizador Lexico. Convierte el texto en simbolos.
  - symbol_table.c / .h:      Tabla de Simbolos. Guarda identificadores y palabras reservadas.
  - analyzer.c / .h:          Invoca al analizador lexico y muestra los resultados.
  - errors.c / .h:            Gestor de errores.
  - definitions.h:            Definiciones de tokens, constantes y la estructura Symbol.
  - main.c:                   Orquesta todo el proceso de compilacion.


RESUMEN DEL DISENO
=======================================
1. SISTEMA DE ENTRADA (input_system):
   Doble buffer con centinela: dos bloques de 1024 bytes que se cargan bajo
   demanda con fread(). Un puntero "inicio" marca donde empieza el lexema
   actual y otro "delantero" avanza caracter a caracter. Si un lexema cruza
   la frontera entre bloques, getLexeme() concatena ambas partes. El EOF
   real se distingue del centinela comprobando feof() sobre el fichero.

2. TABLA DE SIMBOLOS (symbol_table):
   Es una Tabla Hash que crece sola (se duplica automaticamente si se llena
   al 70%). Las palabras reservadas de D se cargan al inicio del programa para
   poder diferenciarlas rapido de las variables normales.

3. ANALIZADOR LEXICO (lexical_analyzer):
   El codigo esta dividido en funciones pequenas (para numeros, strings, etc.)
   para no tener un bloque gigante e ilegible. Soporta los comentarios de D;
   para los comentarios anidados usa un simple contador numerico en vez de
   estructuras de memoria complejas.

4. MEMORIA Y ERRORES:
   - Los errores se separan en "Fatales" (abortan el programa, ej. no existe el
     archivo) y "Leves" (avisan por pantalla pero el programa sigue para cazar
     mas fallos).
   - No hay fugas de memoria (0 memory leaks). Toda memoria pedida para los
     lexemas se libera en cuanto el analizador termina con ellos. Al final,
     se limpia toda la tabla.
