CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = analizador
SRCS = main.c AS.c AL.c SI.c TS.c errores.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET) regression.d

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: run clean
