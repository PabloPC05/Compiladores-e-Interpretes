Solo hay que reconocer los números que aparecen en el regression.d Por ejemplo hexadecimales con exponente no hacen falta. 
En los strings hay qe guardar las comillas
Habría que eliminar las variables globales en su mayoria ya que exponene l compilador a ataques. 
El analizador sintáctico imprime el componente léxico y el lexema. 
El analizador sintáctico debe llamar al analizador léxico. El main solo debe llamar al analizador sintáctico. 
Para los comentarios anidados, sería optimo hacerlo con un afd, pero se puede hacer con una pila. 
El manejo erróneo de la memoria dinámica es un error grande. 
La funcion de peek() no debería de usarla, ya que elonsumo de los caracteres deberia de ser lineal. 

En el fichero main hay que convocar la ipresion de la tbla de simbolos con las palabras reservadas y nada más. Y hay que volver a imprimirla al final del análisis. 

Ten en cuenta el caso de que 0bx para considerarlo como posible binario, para que luego se convierta en un '0' y luego una cadena de 'bx'

Las bibliotecas de C adicionales como string o ctype, son indispensables? o podría hacerse lo que hacen de forma nativa

En lugar de imprimir los errores en cada sitio, mejor ten constantes en errores.h y que se llame a una funcion report() para imprimirlos y hacer el código más legible y compacto. 

Ten en cuenta el uso de static inline en las funciones para hacer todo el compilador mucho mas eficiente. 

Piensa si hay nuevas opciones para usar switch en lugar de condicionales en funciones que tengan muchas opciones 