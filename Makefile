# Rosu Mihai Cosmin 333CA
CFLAGS=-Wall -Wextra -Werror

build: LinkedList.o tema1.o
	gcc tema1.o LinkedList.o -o tema1 -lpthread

LinkedList.o:
	gcc $(CFLAGS) LinkedList.c LinkedList.h -c

tema1.o:
	gcc $(CFLAGS) tema1.c -c

clean:
	rm tema1 *.o *.gch