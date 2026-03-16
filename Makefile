CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = analizador
SRCS = main.c analyzer.c lexical_analyzer.c input_system.c symbol_table.c errors.c definitions.c
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
