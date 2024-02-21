# multi-threading-project
A multi-threading project in C
Game: “Hunters Track Evidences and Catch Ghosts”

# description of the program’s purpose:
A functioning, multi-threaded simulation, that contains one ghost thread and four hunter threads. These five entities hang out in the connected rooms in a house. The only ghost could leave evidence in a room, and move to a random, connected room, only if a hunter is NOT in the room. Hunters could collect evidence, and move to a random, connected room. When ghost's boredom timer reaches BOREDOM_MAX, it will end its haunting. When hunters' boredom timer or fearness timer reach the max, they will be removed from the simulation.

# list of files included in the project and their purposes:
README.md :     informations of the program and how to run the program

defs.h :        the header file, contains forward declarations, type definitions, error codes and other definitions
main.c :        the source file, contains main()
house.c :       the source file, contains initHouse(), cleanupHouse(), and populateRooms()
room.c :        the source file, contains all functions related to room entities
ghost.c :       the source file, contains all functions related to the ghost entity
hunter.c :      the source file, contains all functions related to hunter entities
evidence.c :    the source file, contains all functions related to evidence entities
logger.c :      provided logging file, prints messages of threads actions
utils.c :       provided file, contains some random functions and evidenceToString()
Makefile :      for compilation and linking

# Instructions for compiling the program:
make

# Instructions for running the program:
./fp

# Instructions for how to use the program once it is running:
NA
