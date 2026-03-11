#ifndef SI_H
#define SI_H

void inicializar_SI(const char *nombre_fichero);
char sig_caracter(void);
//char peek_caracter(void);
void devolver_caracter(void);
char *get_lexema(void);
void mover_inicio(void);
void cerrar_SI(void);

#endif /* SI_H */
