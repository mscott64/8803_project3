CC=gcc
CFLAGS=-I. -I./client -I./proxy -I./shared -I./server -lpthread -Wall

all: client_ex proxy_ex server_ex

client_ex: obj/client.o
	$(CC) -o $@ $^ $(CFLAGS)

proxy_ex: obj/proxy.o obj/shared.o
	$(CC) -o $@ $^ $(CFLAGS)

server_ex: obj/server.o obj/shared.o
	$(CC) -o $@ $^ $(CFLAGS)

obj/client.o: client/client.c client/client.h constants.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/proxy.o: proxy/proxy.c proxy/proxy.h constants.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/server.o: server/server.c server/server.h constants.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/shared.o: shared/shared.c shared/shared.h constants.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f obj/*.o client_ex proxy_ex server_ex