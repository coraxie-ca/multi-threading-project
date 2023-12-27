#include "defs.h"

/*
  Function: initHunterArr
  Purpose:  initialize the size of hunter array to 0
  Params:  
    Input/Output: HunterArrayType *hunterArr - the hunter array to be initialized
*/
void initHunterArr(HunterArrayType *hunterArr) {
    hunterArr->size = 0;
}

/*
  Function: addHunter
  Purpose:  add a hunter to the given hunter array, update the size of the array
  Params:  
    Input:  HunterType *hunter - the hunter to add
    Input/Output: HunterArrayType *hunterArr - the hunter array to add to
*/
void addHunter(HunterType *hunter, HunterArrayType *arr) {
    arr->hunter[arr->size++] = hunter;
}

/*
  Function: initHunter
  Purpose:  initialize every field of the ghost, call logger to print Hunter Initialization
  Params:  
    Input/Output:   HunterType **hunter - the hunter to initialize
                    HouseType *house - the house that the hunters in
    Input:          char *name - the name of the hunter
                    int equipmentNo - the equipment number from the equipment array of the house, for assigning different equipment to each hunter
*/
void initHunter(char *name, HunterType **hunter, HouseType *house, int equipmentNo) {
    *hunter = (HunterType *) malloc(sizeof(HunterType));
    
    sem_wait(&house->rooms.head->data->mutex); // lock van
        (*hunter)->room = house->rooms.head->data; // update the room pointer of the hunter
        addHunter(*hunter, &(*hunter)->room->hunters); // update the hunter arr of the room that the hunter is in
    sem_post(&house->rooms.head->data->mutex);
    
    strcpy((*hunter)->name, name);
    (*hunter)->evidenceSharedList = &house->evidence;
    (*hunter)->fear = 0;
    (*hunter)->boredom = 0;
    (*hunter)->equipment = house->equipmentSet[equipmentNo];
    
    addHunter(*hunter, &house->hunters); // add hunter to the hunter array of the house
    l_hunterInit((*hunter)->name, (*hunter)->equipment);
}

/*
  Function: cleanupHunterArr
  Purpose:  deallocate momery of hunters in the hunter array
  Params:  
    Input/Output: HunterArrayType *hunterArr - the hunter array which contains hunters to free 
*/
void cleanupHunterArr(HunterArrayType *hunterArr) {
    for (int i = 0; i < hunterArr->size; i++) {
        free(hunterArr->hunter[i]);
    }
    hunterArr->size = 0;
}

/*
  Function: hunterCondition
  Purpose:  update hunter's fear and boredom;
            if a hunter starts their loop in the same room as a ghost, they gain 1 point of fear and reset their boredom to 0;
            if they are not in the room with a Ghost, they increase their boredom by 1.
  Params:  
    Input/Output:   HunterType *hunter - the hunter to update
*/
void hunterCondition(HunterType *hunter) {
    if (hunter->room->ghost != NULL) {
        hunter->fear += FEAR_INC;
        hunter->boredom = 0;
    } else {
        hunter->boredom += BOREDOM_INC;
    }
}

/*
  Function: hunterLeaveRoom
  Purpose:  update hunter array of the room before the hunter leave current room
  Params:  
    Input/Output:   HunterType *hunter - the hunter to update
*/
void hunterLeaveRoom(HunterType *hunter) {
    for (int i = 0; i < hunter->room->hunters.size; i++) {
        if (hunter->room->hunters.hunter[i] == hunter) {
            for (int j = i; j < hunter->room->hunters.size - 1; j++) {
                hunter->room->hunters.hunter[j] = hunter->room->hunters.hunter[j+1];
            }
            hunter->room->hunters.size--;
            break;
        }
    }
}

/*
  Function: hunterMove
  Purpose:  move to a random, connected room
            call logger to print Hunter Movement
            if a hunter starts their loop in the same room as a ghost, they gain 1 point of fear and reset their boredom to 0
            if they are not in the room with a Ghost, they increase their boredom by 1
  Params:  
    Input/Output: HunterType *hunter - the hunter to move
*/
void hunterMove(HunterType *hunter) {
    RoomType *curRoom;
    RoomType *nextRoom;
    RoomNodeType *node; // room node of the room list of where the hunter is
    curRoom = hunter->room;
    node = curRoom->roomList.head;
    int count = randInt(0, curRoom->roomList.size);
    
    while (count > 0) {
        node = node->next;
        count--;
    }
    nextRoom = node->data;

    sem_t *sems[2] = {&curRoom->mutex, &nextRoom->mutex};
    if (sems[0] > sems[1]) {
        sem_t *temp = sems[0];
        sems[0] = sems[1];
        sems[1] = temp;
    }

    sem_wait(sems[0]);
    sem_wait(sems[1]);

        hunterLeaveRoom(hunter);
        hunter->room = nextRoom;
        addHunter(hunter, &nextRoom->hunters); // update the hunter arr in the new room
        hunterCondition(hunter);
    
    sem_post(sems[1]);
    sem_post(sems[0]);

    l_hunterMove(hunter->name, hunter->room->name);

}

/*
  Function: hunterCollect
  Purpose:  collect evidence in a room if evidence is in that room AND the hunter’s equipment matches the evidence type in the room
            call logger to print Hunter Collect
  Params:  
    Input:          HunterType *hunter - the hunter who collects evidence
    Input/Output:   HouseType *house - if an evidence is successfully collected, add it to the shared evidence list of the house
*/
void hunterCollect(HunterType *hunter, HouseType *house) {
    EvidenceNodeType *node; // the evidence node of the room's evdence list
    EvidenceNodeType *preNode;
    EvidenceType newEv; // the evdence data of the above 'node'   
    node = hunter->room->evidenceList.head;
    preNode = NULL;

    EvidenceType detectableEv;
    detectableEv = hunter->equipment; // the evidence that matches this hunter's equipment

    // iterate the room's evdence list
    while (node != NULL) {
        newEv = *node->data;
        if (newEv == detectableEv) {
            
            sem_t *sems[2] = {&house->evidence.mutex, &hunter->room->mutex};
            if (sems[0] > sems[1]) {
                sem_t *temp = sems[0];
                sems[0] = sems[1];
                sems[1] = temp;
            }

            sem_wait(sems[0]);
            sem_wait(sems[1]);
           
                // update the shared evidence list of house
                // if the newEv is already in the evList of the house, then no need to add again
                EvidenceNodeType *checkNode;
                checkNode = house->evidence.head;
                while (checkNode != NULL) {
                    EvidenceType checkData = *checkNode->data;
                    if (checkData == newEv) {
                        break;
                    }
                    if (checkNode == house->evidence.tail) {
                        addEvidence(&newEv, &house->evidence);
                        break;
                    }
                    checkNode = checkNode->next;
                }

                // remove the found evidence from the evidence list of room
                // 'node' is the one to be removed
                if (preNode != NULL) {
                    preNode->next = node->next;
                } else {
                    // preNode = NULL, then the head is removed
                    hunter->room->evidenceList.head = node->next;
                }
                // if the removed 'node' is tail
                if (node->next == NULL) {
                    hunter->room->evidenceList.tail = preNode;
                }

                EvidenceNodeType *nextNode = node->next;
                free(node); // this free cause seg fault, but need free???????
                hunterCondition(hunter);

            sem_post(sems[1]);
            sem_post(sems[0]);

            l_hunterCollect(hunter->name, newEv, hunter->room->name);
            node = nextNode;
        } else {
            preNode = node;
            node = node->next;
        }
    }
}

/*
  Function: hunterReveiw
  Purpose:  review the evidence that is shared between all hunters to determine if the thread should end; 
            it should end if the hunters have collectively gathered 3 unique pieces of evidence;
            call logger to print Hunter Reveiw
  Params:  
    Input:  HunterType *hunter - the hunter who reviews the shared evidence list 
    Input:  HouseType *house - the house which contains the shared evidence list
*/
void hunterReveiw(HunterType *hunter, HouseType *house) {
    EvidenceType evArr[LOG_SUFFICIENT] = {EV_UNKNOWN, EV_UNKNOWN, EV_UNKNOWN};
    int count = 0;
    int boolean = C_TRUE;
    LoggerType result = LOG_INSUFFICIENT;


    sem_t *sems[2] = {&house->evidence.mutex, &hunter->room->mutex};
    if (sems[0] > sems[1]) {
        sem_t *temp = sems[0];
        sems[0] = sems[1];
        sems[1] = temp;
    }

    sem_wait(sems[0]);
    sem_wait(sems[1]);

        EvidenceNodeType *node; // node of shared evList
        node = house->evidence.head;

        while (node != NULL) {
            // iterate the evArr, if the node->ev from shared list is already in the evArr, set false
            for (int i = 0; i < LOG_SUFFICIENT; i++) {
                if (*node->data == evArr[i]) {
                    boolean = C_FALSE;
                    break;
                }
            }
            // if true, add node->ev to evArr, count +1
            if (boolean) {
                evArr[count] = *node->data;
                count++;
            }
            boolean = C_TRUE;
            node = node->next;
        }

        hunterCondition(hunter);

    sem_post(sems[1]);
    sem_post(sems[0]);

    if (count >= 3) {
        result = LOG_SUFFICIENT;
    }   
    l_hunterReview(hunter->name, result);
    
    if (result == LOG_SUFFICIENT) {
        l_hunterExit(hunter->name, LOG_SUFFICIENT);
        pthread_exit(NULL);    }   
}

/*
  Function: hunterThread
  Purpose:  randomly choose to either collect evidence, move, or review evidence;
            if the the fear of the hunter is greater than or equal to FEAR_MAX, exit the thread;
            if the the boredom of the hunter is greater than or equal to BOREDOM_MAX, exit the thread.
  Params:  
    Input:  void* arg - the struct to run the thread, which contains a house pointer and a hunter pointer
*/
void* hunterThread(void* arg) {
    HunterThreadArgType *threadArg = (HunterThreadArgType *) arg;
    HouseType *house = threadArg->house;
    HunterType *hunter = threadArg->hunter;

    while (1) {
        usleep(HUNTER_WAIT);
        int random = randInt(0,3);
        
        // fear and boredom only change when hunter moves, 
        // consider when hunter take any actions (collect, review, do nothing)
        // need lock room when doing so
        if (random == 0) {
            hunterCollect(hunter, house);
        }
        if (random == 1) {
            hunterMove(hunter);
        }
        if (random == 2) {
            hunterReveiw(hunter, house);
        }

        // the hunter exit, should remove the hunter from the hunter array of the room
        if (hunter->boredom >= BOREDOM_MAX) {
            sem_wait(&hunter->room->mutex);
                hunterLeaveRoom(hunter);
            sem_post(&hunter->room->mutex);

            l_hunterExit(hunter->name, LOG_BORED);
            pthread_exit(NULL);
            break;
        }
        if (hunter->fear >= FEAR_MAX) {
            sem_wait(&hunter->room->mutex);
                hunterLeaveRoom(hunter);
            sem_post(&hunter->room->mutex);

            l_hunterExit(hunter->name, LOG_FEAR);
            pthread_exit(NULL);
            break;
        }
    }

    if (house->ghostHasEnd) {
        return NULL;
    }
}