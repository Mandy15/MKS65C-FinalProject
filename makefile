# taboo: taboo.o guess.o hinter.o
# 	gcc -o taboo taboo.o guess.o hinter.o
#
# taboo.o: taboo.c networking.h
# 	gcc -c taboo.c
#
# guess.o: guess.c networking.h
# 	gcc -c guess.c
#
# hinter.o: hinter.c networking.h
# 	gcc -c hinter.c

forking: client fserver

fserver: forking_server.o networking.o
	gcc -o server forking_server.o networking.o

client: client.o networking.o
	gcc -o client client.o networking.o

client.o: client.c networking.h
	gcc -c client.c

forking_server.o: forking_server.c networking.h
	gcc -c forking_server.c

networking.o: networking.c networking.h
	gcc -c networking.c

clean:
	rm *.o clients.txt
