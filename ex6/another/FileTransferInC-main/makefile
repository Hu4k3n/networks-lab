CC = gcc

all: server client

client: client.c
	$(CC) client.c -o client

server: server.c
	$(CC) server.c -o server

clean:
	rm *.o server client