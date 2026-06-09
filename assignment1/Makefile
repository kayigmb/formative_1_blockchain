CC = gcc
CFLAGS = -Wall
LIBS = -lssl -lcrypto

all:
	$(CC) main.c blockchain.c registry.c crypto.c storage.c -o attend $(LIBS)

run:
	./attend

clean:
	rm -f attend
