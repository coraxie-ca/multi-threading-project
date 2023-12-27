#include "defs.h"

/*
  Function: initGhost
  Purpose:  initialize every field of the ghost, call logger to print Ghost Initialization
  Params:  
    Input/Output:   GhostType **ghost - the ghost to initialize
                    HouseType *house  - the house that the ghost in
*/
void initGhost(GhostType **ghost, HouseType *house) {
    *ghost = (GhostType *) malloc(sizeof(GhostType));
    (*ghost)->ghostClass = randomGhost();
    (*ghost)->boreTimer = 0;

    // choose a random start room for the ghost, except van 
    RoomNodeType *node;
    node = house->rooms.head; // van
    int count = randInt(1, house->rooms.size); // [1,13), 13 rooms in total, van is the head

    while (count > 0 && node != NULL) {
        node = node->next;
        count--;
    }

    sem_wait(&node->data->mutex);
        (*ghost)->room = node->data; // update the room pointer of ghost
        node->data->ghost = *ghost; // update the ghost pointer of room
    sem_post(&node->data->mutex);

    l_ghostInit((*ghost)->ghostClass, (*ghost)->room->name);
}

/*
  Function: ghostLeaveEvidence
  Purpose:  adding one random evidence to the room’s evidence collection, call logger to print ghost Leaving Evidence
            the evidence must be part of the ghost’s valid evidence types
            
            POLTERGEIST:    Leaves ghostly EMF, TEMPERATURE, and FINGERPRINTS
            BANSHEE:        Leaves ghostly EMF, TEMPERATURE, and SOUND
            BULLIES:        Leaves ghostly EMF, FINGERPRINTS, and SOUND
            PHANTOM:        Leaves ghostly TEMPERATURE, FINGERPRINTS, and SOUND
  Params:  
    Input:  GhostType *ghost - the ghost who leave evidence
*/
void ghostLeaveEvidence(GhostType *ghost) {
    sem_wait(&ghost->room->evidenceList.mutex);
        while (1) {
            EvidenceType randomEv = (EvidenceType) randInt(0, EV_COUNT);
            
            if (ghost->ghostClass == POLTERGEIST) {
                if (randomEv != SOUND) {
                    addEvidence(&randomEv, &ghost->room->evidenceList);
                    l_ghostEvidence(randomEv, ghost->room->name);
                    break;
                }
            }

            if (ghost->ghostClass == BANSHEE) {
                if (randomEv != FINGERPRINTS) {
                    addEvidence(&randomEv, &ghost->room->evidenceList);
                    l_ghostEvidence(randomEv, ghost->room->name);
                    break;
                }
            }
            
            if (ghost->ghostClass == BULLIES) {
                if (randomEv != TEMPERATURE) {
                    addEvidence(&randomEv, &ghost->room->evidenceList);
                    l_ghostEvidence(randomEv, ghost->room->name);
                    break;
                }
            }

            if (ghost->ghostClass == PHANTOM) {
                if (randomEv != EMF) {
                    addEvidence(&randomEv, &ghost->room->evidenceList);
                    l_ghostEvidence(randomEv, ghost->room->name);
                    break;
                }
            }
    }
    sem_post(&ghost->room->evidenceList.mutex);
}

/*
  Function: ghostMove
  Purpose:  move to a random, connected room, only if a hunter is NOT in the room
            if a hunter is in the room, the ghost will only choose to do nothing or leave evidence
            call logger to print ghost Movement
  Params:  
    Input/Output: GhostType *ghost - the ghost to move
*/
void ghostMove(GhostType *ghost) {
    RoomType *curRoom;
    RoomType *nextRoom;
    RoomNodeType *node;
    node = ghost->room->roomList.head;
    int count = randInt(0, ghost->room->roomList.size);
    
    while (count > 0) { // if count = 0, this while loop will not execute, then node = roomList.head
        node = node->next;
        count--;
    }
    nextRoom = node->data;
    curRoom = ghost->room;

    sem_t *sems[2] = {&curRoom->mutex, &nextRoom->mutex};
    if (sems[0] > sems[1]) {
        sem_t *temp = sems[0];
        sems[0] = sems[1];
        sems[1] = temp;
    }

    sem_wait(sems[0]);
    sem_wait(sems[1]);
        if (nextRoom->hunters.size == 0) { // ghost moves to a new room, no hunter, boreTimer++
            curRoom->ghost = NULL; // update the ghost pointer of curRoom
            ghost->room = nextRoom; // update the room pointer of the ghost
            nextRoom->ghost = ghost; // update the ghost pointer of nextRoom
            ghost->boreTimer += BOREDOM_INC;
            l_ghostMove(nextRoom->name);
        } else {
            if (randInt(0,2) == 0) {
                ghostLeaveEvidence(ghost);
            }
        }
    sem_post(sems[1]);
    sem_post(sems[0]);
}

/*
  Function: ghostThread
  Purpose:  if the Ghost is in the room with a hunter, reset the Ghost’s boredom timer to 0, and it cannot move. Randomly choose to leave evidence or to do nothing.
            if the Ghost is not in the room with a hunter, increase the Ghost’s boredom counter by 1. Randomly choose to move to an adjacent room, to leave evidence, or to do nothing.
            if the ghost’s boredom counter has reached BOREDOM_MAX, exit the thread.
  Params:  
    Input:  void* arg - the struct to run the thread, which contains a house pointer and a ghost pointer
*/
void* ghostThread(void* arg) {
    GhostThreadArgType *threadArg = (GhostThreadArgType *) arg;
    HouseType *house = threadArg->house;
    GhostType *ghost = threadArg->ghost;

    while (1) {
        usleep(GHOST_WAIT);

        if (ghost->room->hunters.size != 0) {
            ghost->boreTimer = 0;
            if (randInt(0,2) == 0) {
                ghostLeaveEvidence(ghost);
            }
        }

        if (ghost->room->hunters.size == 0) {
            ghost->boreTimer += BOREDOM_INC;
            int random = randInt(0,3);
            if (random == 0) {
                ghostLeaveEvidence(ghost);
            } else if (random == 1) {
                ghostMove(ghost);
            }
        }

        if (ghost->boreTimer >= BOREDOM_MAX) {
            l_ghostExit(LOG_BORED);
            pthread_exit(NULL);
            break;
        }
    }
    house->ghostHasEnd = C_TRUE; 
    return NULL;
}
