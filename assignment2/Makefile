CC = gcc
CFLAGS = -Wall
LIBS = -lssl -lcrypto

all:
	$(CC) $(CFLAGS) main.c blockchain.c registry.c crypto.c storage.c transaction.c utxo.c mining.c -o assignment2_build $(LIBS)

run:
	./assignment2_build

clean:
	rm -f assignment2_build 
