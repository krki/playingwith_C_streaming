CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LIBS = -lm -mconsole

# Arquivos fonte
SOURCES = main.c csvutil.c content.c user.c list.c recommendation.c report.c
TEST_SOURCES = test.c csvutil.c content.c user.c list.c recommendation.c report.c

# Objetos
OBJECTS = $(SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

# Executáveis
MAIN = streamflix
TEST = test_streamflix

all: $(MAIN)

test: $(TEST)
	./$(TEST)

# Regra para compilar o executável principal
$(MAIN): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para compilar o executável de testes
$(TEST): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Regra para compilar arquivos .c em arquivos .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(MAIN) $(TEST)

.PHONY: all test clean