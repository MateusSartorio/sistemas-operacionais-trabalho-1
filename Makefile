NOME_PROJETO=trab1

CC=gcc
CFLAGS=-c

# arquivos .c
C_SOURCE=$(wildcard ./src/*.c)

# arquivos .h
H_SOURCE=$(wildcard ./headers/*.h)

# arquivos objeto .o
OBJ=$(C_SOURCE:./src/%.c=./bin/%.o)

all: | DIR

DIR:
	mkdir -p bin

all: $(NOME_PROJETO)

$(NOME_PROJETO): $(OBJ)
	$(CC) -o $@ $^

./bin/main.o: ./src/main.c $(H_SOURCE)
	$(CC) $(CFLAGS) -o $@ $<

./bin/%.o: ./src/%.c ./headers/%.h
	$(CC) $(CFLAGS) -o $@ $<

run:
	./$(NOME_PROJETO)

clean:
	rm -f ./bin/*.o $(NOME_PROJETO)