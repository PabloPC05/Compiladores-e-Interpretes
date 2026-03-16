#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

void initInput(const char *filename);
int  nextChar();
void returnChar();
char *getLexeme();
void moveInicio();
int  getLine();
int  getColumn();
void closeInput();

#endif /* INPUT_SYSTEM_H */
