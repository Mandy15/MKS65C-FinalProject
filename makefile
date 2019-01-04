all: taboo.o guess.o hinter.o
	gcc -o taboo taboo.o guess.o hinter.o

taboo.o: taboo.c taboo.h
	gcc -c taboo.c

guess.o: guess.c guess.h
	gcc -c guess.c

hinter.o: hinter.c hinter.h
	gcc -c hinter.c

clean:
	rm taboo *.o

run:
	./taboo
