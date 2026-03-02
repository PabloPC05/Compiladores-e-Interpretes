Dentro de la entrega hay que incluir el archivo de regression.d, se podría organizar el código en carpetas. 
Estaria bien incluir un readme o .md de cuestiones que creamos que sean útiles para la práctica. Es decir, hay que incluir documentación en general. 
Habría que inclir un makefile para que permita una fácil compilación del códgio para facilitarlo al corregidor. No debería haber warinings. La versión final o para entregar  deberia funcionar para una versión linux de las últimas, parecidas a las de ubuntu. Puede haber problemas ed compilacion a pesar de que compile en windows. 
El código debe estar *muy bien comentado y documentado*. No hay que incluir el nombre o ningun tipo de identificacion
A la carpeta en la que inlcuimos el código no hay que ponerle el nombre, el campus virtual ya lo hace por nosotros. Entreguemoslo directamente en un .zip
Se espera que el caracter EOF sea transformado por el analizador léxico en el componente léxico EOF, para que finalice la ejecución.
En la etapa de finalización en el main, debe limpiar la memoria, no pueden quedar punteros colgantes o usos de memoria inncesarios. Podemos usar la herramienta valgreed? Hay que apuntar a null los punteros que queden por ahi.
Cada componente léxico debe tener un struct de 2 componentes, el token y el número que lo representa. 
No se pueden usar estructuras estáticas en la version final. 
Estaría bien hacer que aunque la práctica esté enfocada a regression.d, el código no haya que hacerlo super especificamente para ese archivo, si no para en general cualquier archivo del lenguaje .d Para ello no hace falta incluir todo el léxico del lenguaje d, sólo es necesario el de regression, pero estaría bien incluir todas las palabras. 

El analizador léxico lee caracteres del código. El analizador léxico devuelve numeros enteros al analizador sintáctico para que compruebe si es gramaticalmente correcto. 
El lexema es la palabra en sí que leemos y el componente léxico es el numero unívico que lo identifica. 

El autómata que se encarga de reconocer las componentes léxicas es un afd. Este automata solo tiene por entrada caracteres. 
se pasa de q0 a q1 con [a, ..Z] y se queda ahi mientras sea [a, Z]. Luego pasamos a q2 si la entrada es cualquier cosa que no sea un caracter. Para que este automata reconozca tambien identificadores es necesario incluir números. Hay que incluir los números en sólo la segunda transicion ya que en el lenguaje d, los identificadores no pueden empezar por un número. Con esto tenemos un autómata que reconoce los identificadores y las palabras reservadas. 
No se puede hacer un autómata que reconozca sólo identificadores o solo palabras reservadas. Para diferenciarlos tenemos la tabla de símbolos, la cual es llamada por el analizador léxico para diferenciar si es un identificador o una palabra reservada. 
Nuestro autómata reconoce cadenas que incluyen un último caracter que no es alfanumérico, como "import " o "std." cuando no importa ese último caracter. 


Para reconocer los comentarios tenemos otro autómata finito determinista, que comienza reconociendo "/" y llega al estado final reconociendo el caracter "/". Habiendo estados intermedios que no importan del todo. 

Los lexemas o caracteres que permiten una ambiguedad tales como "<=" o "<<" pueden signficiar una salida booleana o una salida por consola. Habiendo leído solo el primero de los caracteres. Para poder diferenciarlos cuanto antes, tenemos que intentar diferenciarlos desde el principio, no bifurcar desde el priemr caracter como si fuera un árbol. 


Para analizar los números podríamos agrupoar los números entre enteros o números de punto flotante. Dentro del primero agrupamos los binarios, hexadecimales, decimales, etc.

Nuestro analizador léxico debe ser una tabla que reconozca entradas que estén dentro del código ASCII. El analizador léxico cuando el automata no reconoce un caracter o tras una entrada no existe estado al que ir, salta un error de lectura. 

Para cada caracter leído, hay que probarlo en cada uno de los autómatas para saber si es posible reconocerlo en el lenguaje dado. Aunque dé error en uno de los autómatas hay que problarlo en todos, dado que puede estar dentro de un principio de cadena que da lugar a otro componente léxico de la tabla de símbolos. 


Para unificar autómatas hacemos un estado común inicial para que distinga según la primera entrada cual es el autómata que hay que reconocer. Gracias a esto, si la priemra entrada no cuadra para ninguna de las opciones, debe dar error léxico. 

Los strings se reconocen fácilmente por los autómatas gracias a las comillas dobles. 

Estos autómatas dado que forman parte del analizador léxico no necesitan almacenar en memoria lo anterior, pues el estado actual representa la memoria. Solo sirven para identificar no para interpretar. 

A pesar de que los autómatas estén fusionados en uno sólo en la teoría. En la práctica hay que modularizar y hacer una función para cada una de los posibles lexemas para ganazar legibilidad, facilidad para debugear, extensibilidad, etc. 

Para la lectura de por ejemplo, 0b0x3, dado que es código en D, que viene establecido que tras el 0b o 0B no debería tener mas que '0' o 1'. Dado que para la cadena dada, el autómata que reconoce números binarios, el autómata daría un error al leer la 'x' y devolvería un error. A pesar de este error, nuestro compilador debería seguir leyendo el resto del código ya que debe reconocer todos los errores posibles en el código. 
Para este error en concreto se debería interpretar que el analizador léxico reconoce 0b0 y por otro lado x3, que lo reconoce como identificador, lo pasa al analizador sintáctico y es el mismo el que devuelve el error, pues da error bajo el contexto en el que estamos, que es tener un número binario. 
Por eso son mas comunes los errores sintácticos (syntax error), mientras que los errores léxicos son mucho menos comunes. Reconocer que el error anterior es debido a eso, depende totalemente de cómo estén diseñados los analizadores, por ejemplo, el compilador de GO, reconocería que la cadena anterior, sería un error léxico y no sintáctico. 



Sólo hay un ámbito desde que empieza el código hasta que termina. 
Es muy importante que la tabla de símbolos esté representada por una ED dinámica, ppues puede cambiar el archivo que estamos compilando. Puede ser una tabla hash o un árbol binario de búsqueda. Si usamos una hash es importante usar una funcon hash lo suficientemente potente ya que en caso de que usemos enlazada por lista, puede ocurrir que sólo haya una posiicon con una lista larguísima. Hay que usar una hash estudiada. 


Cuando el main inicializa la tabla de símbolos hay que imprimir en consola las palabras reservadas para saber que se han guuardadado bien. Al final de la ejecución tambien hay que imprimir los identificadores para saber que se han guardado bien a lo largo de la compilación. Hay que limpiar también toda la información de la tabla de símbolos al final, para garantizar la eficiencia de la memoria. 