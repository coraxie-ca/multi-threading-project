# multi-threading-project
The coursework of a multi-threading project in C
Game: “Hunters Track Evidences and Catch Ghosts”

# description of the program’s purpose:
A functioning, multi-threaded simulation, that contains one ghost thread and four hunter threads. These five entities hang out in the connected rooms in a house. The only ghost could leave evidence in a room, and move to a random, connected room, only if a hunter is NOT in the room. Hunters could collect evidence, and move to a random, connected room. When ghost's boredom timer reaches BOREDOM_MAX, it is too bored, and ends its haunting. When hunters feel too bored or too scared, they are removed from the simulation.

# list of files included in the project and their purposes:
README.md :     informations of the program and how to run the program
defs.h :        the header file, contains forward declarations, type definitions, error codes and other definitions
main.c :        the source file, contains main()
house.c :       the source file, contains initBuilding(), cleanupBuilding(), loadBuildingData(), to initialize a building structure that contains a list of every ghost and an array every room, like a big database, to clean up all the data, and load the data
room.c :        the source file, contains all functions related to Room structure and RoomArray structure
ghost.c :       the source file, contains all functions related to Ghost structure and GhostList structure
hunter.c :      
evidence.c :     
logger.c :      
utils.c :       
Makefile :      for compilation and linking

# Instructions for compiling the program:
make

# Instructions for running the program:
./fp

# Instructions for how to use the program once it is running:
