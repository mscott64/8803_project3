CC=gcc
CFLAGS=-I. -I./client -lpthread -Wall

all: client_ex

client_ex: obj/client.o
	$(CC) -o $@ $^ $(CFLAGS)

obj/client.o: client/client.c client/client.h constants.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f obj/*.o client_ex