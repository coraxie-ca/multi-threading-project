#include "defs.h"


/*
  Function: initHouse
  Purpose:  initialize every field of the house
  Params:  
    Input/Output: HouseType *house - the house to be initialized
*/
void initHouse(HouseType *house) {
    house->ghostHasEnd = C_FALSE;
    initHunterArr(&house->hunters);
    initRoomList(&house->rooms);
    initEvidenceList(&house->evidence);

    house->equipmentSet[0] = EMF;
    house->equipmentSet[1] = TEMPERATURE;
    house->equipmentSet[2] = FINGERPRINTS;
    house->equipmentSet[3] = SOUND;
}

/*
  Function: cleanupHouse
  Purpose:  deallocate all momery of the house struct, including the rooms, a ghose, hunters, roomList and evidenceList
  Params:  
    Input/Output: HouseType *house - the house to free
*/
void cleanupHouse(HouseType *house) {
    cleanupHunterArr(&house->hunters);
    // cleanupEvidenceList(&house->evidence);

    RoomNodeType *curNode;
    RoomNodeType *nextNode;
    curNode = house->rooms.head;

    while (curNode != NULL) {
        nextNode = curNode->next;
        cleanupRoom(curNode->data);
        curNode = nextNode;
    }
    cleanupRoomList(&house->rooms);
    
    free(house->ghost);
}

/*
  Function: populateRooms 
  Purpose:  create each room, add each room to each other's room lists and add each room to the house's room list
  Params:  
    Input/Output: HouseType *house - the house which contains the rooms to add and connect
*/
void populateRooms(HouseType* house) {
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}