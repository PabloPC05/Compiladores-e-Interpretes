Hay que implementar una tabla hash específica, no se pueden incluir paquetes externos, si no que hay que hacerla o descargarla e incluirla como un objeto mas. 
La practica tiene una valoracion de 2 puntos sobre 5 de los puntos totales de los laboratorios, la siguiente 0.5, la tercera 1.5 y la útima 1 punto. A esos 5 puntos se suman los 5 puntos de la teoría.
Si se suspenden las prácticas o la teoría, se tienen aprobadas para toda oportunidad siguiente. 
Las segunfas oportuniades de las prácticas son mucho mas exigentes que en la primera oportunidad. 
Hay que incluir como archivos: errores.c errores.h, main.c, TS.c, TS.h, definiciones.h
Vamos a poner una restriccion en la memoria, no se puede almacenar el código fuente en memoria, si no que para leer el código fuente debe existir el sistema de entrada. 
En el sistema de entrada es necesario construir un doble centinela, es decir, un doble buffer, que tiene en su posición final un EOF. El doble centinela aporta una estructura circular, para el que cuando se solicita el siguiente caracter, se pregunta si estamos en el EOF o en un caracter que nos importe. 
El fichero main.c ebe tener una linea que detemrina el comienzo de la compilación. Tenemos que programar un consumidor a través de un lazo. Luego va la compilación y por último la finalización, en la que se produce la limpieza de memoria, con el apuntado de punteros a null, y la eliminiaccion de memoria. DEspues de haber realizado el analisis léxico es necesario imprimir la tabla de símbolos, para ver que están los identificadores y las palabras reservadas, para que así se vea que se ha hecho un correcto desarollo de la tabla de símbolos. 
El analizador léxico le pide al sistema de entrada (gracias al patrón productor-consumidor) el siguiente caracter a través de una función denominada sig.caracter(). Esta función debe ser muy eficiente, ya que al analizador léxico no le importa lo que haya en el código fuente, solo quiere el caracter. 

La estructua de los buffers debe ser un arary fisico, con dos arrays digitales, de capacidad estática o dos arrays de capacidad estática. Usamos un getc() para incluir cada uno de los caracteres de los dos arrys. Tienen que ser consecutivos, es decir, primero el rellenamos el primer array de 1024 caracteres y cuando esté lleno rellenamos los siguientes 1024. Para implementarlo hacemos un bucle de 1024 iteraciones. Lo malo es que eso extremadamnete inefciente, habría que usar fread() con el tamaño de bloque dado. Esa o cualquier otra funcion que permita eso. El orden de carga y lectura, es que despues de una lectura, no se carga otra vez hasta que se le exiga nuevamente los caracteres. La operacion general tiene que ser cargo y leo en bloque repetidamente. Por lo tanto, no se carga el segundo array hasta que se le pida la información. No se inicializan los dos arrays a la vez, si no que se cargan caracteres cuando se le piden. 

A continuación, se crean dos puntores, delantero e inicio. Con la carga de la priemra palabra, import, solo se lee 'i' y se carga en el autómata hasta que reconozca alguna de las palabras identificactivas del autómata. Hay que pensar si el puntero delantero hay que hacerlo de forma leer y desplazar o al revés. Después de que se procese la palabra entera de import, se carga el segundo array. El analizador léxico le pide al sistema de entrada el siguiente caracter pero no es el que le tiene que pedir que lo haga, debe hacerlo él mimso por autonomía propia. 
Después de leer import se llama a la tabla de símbolos, etc. Cada vez que se pida un caracter se debe comprobar que no es el final del bloque ni que tampoco sea el final del fichero en código d. Al tomar un EOF, hay que comprobar que sea un final de fichero o de bloque. SE hace la comprobación sabiendo que los arrays son estáticos, de tamaño fijo. 



Cuando se da un separador, como por ejemplo al leer la cadena 'std.' lo que hacemos es que al iniciar su lectura, tenemos que el puntero inicio está en la 's', a continuación se leen el resto de caracteres hasta el '.' Entonces se le manda al analizador lexico y acepta la cadena gracias a a los autómatas alfanuméricos. Por último, el sistema de entrada mueve el puntero inicio a donde está el delantero. Este caso se complica cuando por ejemplo las cadenas a leer son por ejemplo strings, como '"ab"; ' ya que en este caso, el momento en el que lo acepta el autómata es cuando se leen las segundas comillas. Pero cuando se lee esto, ha yque mandar dos veces el '.' en el caso anterior. 

Son necesarios los dos punteros, ya que si no sería necesario un tercer array estático y sería un desperdicio de memoria. El realloc es la operación mas costosa. 


Hay que usar el número de copias del lexema tras leerlo de fomra que sea lo mas eficiente posible. 

El tamaño máximo de los identificadores, dependen mucho del tipo que sea. Los strings no tienen esa restricción, pero los identificadores si, ya que en caso de que el buffer sea de tamaño 6, y tuvieramos dos, entonces la palabra importantess, no sería reconocida. 


Debe haber un error en el compilador que consista en que los identificadores no pueden superar detemrinada lognitud de nombre. Hay que intentar resolver el problema para que no ocurra el problema, o si no hay solucion, implementar el error que se comenta. Otro tipo de error posible, es que el sistema de entrada no sea capaz de leer una cadena como a = 56 + 267 + 3456; pues puede llevar a error de lectura porque se ha eliminado un bloque. 

El tamaño de bloque ideal para probar para esta práctica sería el numero de caracteres total del archivo regression.d; Auqnue sería ideal que fueramos probando diferentes tamaños mas pequeños para ver si funciona. Una idea sería tomar el identificador más largo como tamaño de los arrays. 

