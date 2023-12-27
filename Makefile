OPT = -Wall -Wextra
OBJ = main.o ghost.o house.o room.o evidence.o hunter.o logger.o utils.o

fp:	$(OBJ)
	gcc -o fp $(OBJ)

main.o:	main.c defs.h 
	gcc $(OPT) -c main.c

house.o: house.c defs.h
	gcc $(OPT) -c house.c

room.o: room.c defs.h
	gcc $(OPT) -c room.c

ghost.o: ghost.c defs.h
	gcc $(OPT) -c ghost.c

evidence.o: evidence.c defs.h
	gcc $(OPT) -c evidence.c

hunter.o: hunter.c defs.h
	gcc $(OPT) -c hunter.c

logger.o: logger.c defs.h
	gcc $(OPT) -c logger.c

utils.o: utils.c defs.h
	gcc $(OPT) -c utils.c

clean:
	rm fp $(OBJ)
