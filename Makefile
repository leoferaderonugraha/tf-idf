CC = g++
CFLAGS = -Wall -g
EXE = tf-idf


all: main
prod: main
	strip $(EXE)

main: obj
	$(CC) $(CFLAGS) -o $(EXE) main.o

obj:
	$(CC) $(CFLAGS) -c main.cpp


clean:
	rm -f *.o $(EXE)
