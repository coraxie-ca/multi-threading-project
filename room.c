#include "defs.h"

/*
  Function: initRoomList
  Purpose:  initialize the room list by setting the head and tail to NULL, size is 0
  Params:  
    Input/Output: RoomListType *roomList - the roomList to be initialized
*/
void initRoomList(RoomListType *roomList) {
    roomList->head = NULL;
    roomList->tail = NULL;
    roomList->size = 0;
}

/*
  Function: createRoom
  Purpose:  initialize every field of the room 
  Params:  
    Input:  char *name - the name of the created room
  Return:   RoomType* - a pointer of the created room
*/
RoomType* createRoom(char *name) {
    RoomType *room;
    room = (RoomType *)malloc(sizeof(RoomType));
    
    initRoomList(&room->roomList);
    initEvidenceList(&room->evidenceList);
    initHunterArr(&room->hunters);
    strcpy(room->name, name);
    room->ghost = NULL;
    sem_init(&room->mutex, 0, 1); 
    return room;
}

/*
  Function: addRoom
  Purpose:  append a room to the end of the room list
  Params:  
    Input/Output:   RoomListType *roomList - the roomList to append to
    Input:          RoomType *room - the room to append
*/
void addRoom(RoomListType *roomList, RoomType *room) {
    RoomNodeType *node;
    node = (RoomNodeType *)malloc(sizeof(RoomNodeType));
    node->data = room;
    node->next = NULL;
    roomList->size += 1;

    if (roomList->head == NULL) {
        roomList->head = node;
        roomList->tail = node;
    } else {
        roomList->tail->next = node;
        roomList->tail = node;
    }
}

/*
  Function: connectRooms
  Purpose:  add each room to each other's room lists
  Params:  
    Input/Output: RoomType *room1 - add room1 to the room list of room2
    Input/Output: RoomType *room2 - add room2 to the room list of room1
*/
void connectRooms(RoomType *room1, RoomType *room2) {
    addRoom(&room1->roomList, room2);
    addRoom(&room2->roomList, room1);
}

/*
  Function: cleanupRoomList
  Purpose:  deallocate momery of all the nodes in roomList
  Params:  
    Input/Output: RoomListType *roomList - the roomList to free nodes in
*/
void cleanupRoomList(RoomListType *roomList) {
    roomList->size = 0;
    RoomNodeType *curNode;
    RoomNodeType *nextNode;
    curNode = roomList->head;

    while (curNode != NULL) {
        nextNode = curNode->next;
        free(curNode);
        curNode = nextNode;
    }
}

/*
  Function: cleanupRoom
  Purpose:  deallocate momery of room
  Params:  
    Input/Output: RoomType *room - the room to free 
*/
void cleanupRoom(RoomType *room) {
    if (room != NULL) {
        cleanupRoomList(&room->roomList);
        cleanupEvidenceList(&room->evidenceList);
        free(room);
    }
}