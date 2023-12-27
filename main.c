#include "defs.h"

int main()
{
    // Initialize the random number generator
    srand(time(NULL));
    
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    GhostType *ghost;
    initGhost(&ghost, &house);
    house.ghost = ghost;

    HunterType *hunters[NUM_HUNTERS];    
    char names[NUM_HUNTERS][MAX_STR];

    for (int i = 0; i < NUM_HUNTERS; i++) {
        printf("Enter the name of hunter %d: ", i+1);
        scanf("%s", names[i]);
        initHunter(names[i], &hunters[i], &house, i);
    }

    // EvidenceType randomEv = (EvidenceType) EMF;
    // addEvidence(&randomEv, &house.rooms.head->data->evidenceList);
    // hunterCollect(hunters[0], &house);

    HunterThreadArgType hunterT1;
    hunterT1.house = &house;
    hunterT1.hunter = hunters[0];

    HunterThreadArgType hunterT2;
    hunterT2.house = &house;
    hunterT2.hunter = hunters[1];

    HunterThreadArgType hunterT3;
    hunterT3.house = &house;
    hunterT3.hunter = hunters[2];

    HunterThreadArgType hunterT4;
    hunterT4.house = &house;
    hunterT4.hunter = hunters[3];

    GhostThreadArgType ghostT;
    ghostT.ghost = ghost;
    ghostT.house = &house;

    pthread_t threadGhost, threadHunter1, threadHunter2, threadHunter3, threadHunter4;
    pthread_create(&threadGhost, NULL, ghostThread, &ghostT);
    pthread_create(&threadHunter1, NULL, hunterThread, &hunterT1);
    pthread_create(&threadHunter2, NULL, hunterThread, &hunterT2);
    pthread_create(&threadHunter3, NULL, hunterThread, &hunterT3);
    pthread_create(&threadHunter4, NULL, hunterThread, &hunterT4);

    pthread_join(threadGhost, NULL);
    pthread_join(threadHunter1, NULL);
    pthread_join(threadHunter2, NULL);
    pthread_join(threadHunter3, NULL);
    pthread_join(threadHunter4, NULL);

    // finalize results
    char ghostName[MAX_STR];
    ghostToString(ghost->ghostClass, ghostName);

    printf("=====================================\n");
    printf("All done! Let's tally the results.\n");
    printf("=====================================\n");

    cleanupHouse(&house);


    return 0;
}
