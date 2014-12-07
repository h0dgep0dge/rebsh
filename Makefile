all: rebsh test

rebsh: rebsh.c prompt.o
	gcc -o rebsh rebsh.c prompt.o

test: test.c prompt.o
	gcc -o test test.c prompt.o

prompt.o: prompt.h prompt.c
	gcc -c prompt.c

clean:
	rm -f *.o rebsh test
