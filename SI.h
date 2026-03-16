#ifndef SI_H
#define SI_H

void inicializar_SI(const char *nombre_fichero);
int  sig_caracter(void);
void devolver_caracter(void);
char *get_lexema(void);
void mover_inicio(void);
int  obtener_linea(void);
int  obtener_columna(void);
void cerrar_SI(void);

#endif /* SI_H */
