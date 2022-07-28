#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shm_header.h"

void print_usage() {
    printf("input format: op data\n"
           "operation: \n 0 -- insert\n 1 -- print bucket\n 2 -- delete\n 3 -- print hashmap (data can be given random)\n");
}

int main(int argc, char const *argv[]) {
    print_usage();
    void *shm;
    int shmid;
    struct message *me;

    shmid = shmget((key_t) 6666, sizeof(struct message) * SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(-1);
    }

    shm = shmat(shmid, (void *) 0, 0);
    if (shm == (void *) (-1)) {
        perror("shmat");
        exit(-1);
    }
    me = (message *) shm;
    while (1) {
        int index = poll_client(me);
        if (index != -1) {
            printf("input operation and data: \n");
            scanf("%d %d", &(me + index)->operation, &(me + index)->data);
            (me + index)->read = false;
        }
    }

    return 0;
}