CC = gcc
CFLAGS = -I./server -Wall

all: server_bin client_bin

# Server requires server.c, game.c and cJSON.c
server_bin: server/server.c server/game.c server/cJSON.c
	$(CC) $(CFLAGS) -o battle server/server.c server/game.c server/cJSON.c

# Client requires client.c AND cJSON.c (since we used cJSON in client)
client_bin: client/client.c server/cJSON.c
	$(CC) $(CFLAGS) -o client_bin client/client.c server/cJSON.c

clean:
	rm -f battle client_bin