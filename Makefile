# Makefile para el analizador lexico del lenguaje D
#
# Uso:
#   make          - Compila el proyecto
#   make clean    - Elimina los archivos generados
#   make run      - Compila y ejecuta con regression.d
#
# Requisitos: gcc (probado en Ubuntu/Linux)

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
TARGET = analizador

# Archivos fuente y objetos
SRCS = main.c analizador.c tabla_simbolos.c sistema_entrada.c errores.c
OBJS = $(SRCS:.c=.o)

# Regla principal: compilar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regla generica: compilar cada .c a .o
%.o: %.c definiciones.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar y ejecutar con el fichero de prueba
run: $(TARGET)
	./$(TARGET) regression.d

# Limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean run
