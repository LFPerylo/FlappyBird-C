CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Arquivo de saída
TARGET = jogopif

# Arquivos fonte
SOURCES = main.c screen.c keyboard.c timer.c

# Arquivos objeto
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	