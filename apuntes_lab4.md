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

---

## Tareas pendientes

1. **[Analizador sintactico]** Crear AS.h y AS.c. El analizador sintactico llama al lexico (sig_comp_lexico) e imprime cada componente lexico y su lexema. ✅

2. **[Main]** Refactorizar main.c: solo debe llamar al analizador sintactico. Inicializar la tabla de simbolos en el main, imprimirla con las palabras reservadas al inicio, y volver a imprimirla al final del analisis. ✅

3. **[Strings]** En el analizador lexico, guardar las comillas como parte del lexema de los literales string.✅

4. **[Variables globales]** Eliminar la mayoria de variables globales del proyecto. Encapsularlas con static o pasarlas como parametros.✅

5. **[Numeros]** Solo reconocer los numeros que aparecen en regression.d. No hace falta reconocer hexadecimales con exponente. ✅

6. **[Binarios]** Tratar el caso de "0bx" (binario invalido): devolver '0' como literal entero y dejar "bx" como identificador separado.

7. **[Comentarios anidados]** Revisar la implementacion de comentarios anidados /+ +/. Valorar usar un AFD en lugar de pila si es posible.

8. **[Errores]** Centralizar errores: definir constantes en errores.h y crear una funcion report() que reciba el codigo de error y lo imprima. Eliminar fprintf dispersos. ✅

9. **[Memoria dinamica]** Revisar todo el manejo de malloc/free. Asegurar que no hay fugas ni dobles liberaciones.

10. **[peek()]** Eliminar o no usar la funcion peek_caracter(). El consumo de caracteres debe ser lineal.

11. **[static inline]** Valorar el uso de static inline en funciones auxiliares internas para mejorar la eficiencia. ✅

12. **[switch vs if]** Revisar funciones con muchos condicionales (especialmente en AL.c) y sustituir cadenas de if-else por switch donde sea posible.

13. **[Dependencias de librerias]** Evaluar si se pueden eliminar dependencias de string.h y ctype.h implementando las funciones necesarias de forma nativa.

14. **[Longitud de los lexemas]** Hay que tener en cuenta la posible longitud de los lexemas, ya que pueden dar lugar a errores 