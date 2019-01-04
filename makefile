all: taboo.o
	gcc -o taboo taboo.o

taboo.o: taboo.c taboo.h
	gcc -c taboo.c

clean:
	rm taboo *.o

run:
	./taboo
