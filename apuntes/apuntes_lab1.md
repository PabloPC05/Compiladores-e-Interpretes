El código fuente viene del archivo regression.d y vamos a realizar el análisis léxico del mismo. 
Una vez leemos el import buscamos la referencia léxica al mismo y vemos que es una palabra reservada (i.e. que no se puede usar para nombrar variables). 
Su lexema es import (la palabra original en el codigo fuente). 
Ejemplo extra: 
a = 2 + 3; 
c = 1 + 1; 
Léxicamente son diferentes, sintácticamente son idénticos. 
Observacion: Los espacios no tienen significado lexico, son meros separadores 

A continuación va la lectura de "std.studio;" En dicha cadena encontramos 3 elementos léxicos, "std", ".", "studio". A diferencia del import que es una palabra reservada, std y stdio son identificadores. 

El analizador sintáctico (consumidor - quien toma la iniciativa) pide el siguiente componente léxico a través de la funcion sig_comlexico() y en analizador léxico (productor) le devuelve el componente léxico: hay que hacer un struct en el que haya un int y un char[]. 

El recorrido del programa es que el analizador sintáctico llama al analizador léxico a través de la función sig_compléxico() y le devuelve <300, "import">. Vuelve a llamarle a través de la función y le devuelve <301, "std">, etc. 
El analizador sintáctico es un while que devuelve los structs correspondientes hasta que encuentra el fin de fichero (EOF).

Se distingue si los componentes léxicos son identificadores o no a través de la tabla de símbolos. 

Hay que llamar a una función buscar() para encontrar su símbolo. En un principio se hace buscar import y devuelve el componente léxico que representa una palabra reservada. Si no está insertada en la tabla de símbolos la insertamos. 

Las palabras reservadas se guardan en la tabla de símbolos, al inicializar la tabla y se hace en el main, en donde se tienen 3 fases: inicialización, llamada al inicio de la compilacion de la entrada y fin. En el final, limpiamos la memoria al finalizar. 

Tenemos que al iniciar el programa llamar al constructuro/inicializador de la tabla de símbolos para incluir las palabras reservadas. 

Al leer std, como no estaba en la tabla de símbolos debemos insertarla en la tabla con el componente identificador. 