all: word_association.o
	gcc -o word_association word_association.o

word_association.o: word_association.c word_association.h
	gcc -c word_association.c

clean:
	rm word_association *.o

run:
	./word_association
