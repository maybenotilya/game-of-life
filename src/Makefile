CC=gcc

all: gameoflife gol_serial
gameoflife: gol_parallel.c io.c 
	$(CC) -o gol_parallel gol_parallel.c io.c -lpthread -pthread -fsanitize=thread -g

