#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "lockhashmap.c"
#include "shm_header.h"

void clean_shm(struct message *me) {
    for (int i = 0; i < SIZE; ++i) {
        (me + i)->read = true;
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 2)
        printf("Usage: ./server <number of buckets>\n");
    HM *hm = alloc_hashmap(atoi(argv[1]));
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
    printf("server start\n");
    clean_shm(me);
    while (1) {
        int index = poll_server(me);
        if (index != -1) {
            struct message *tmp = me + index;
            printf("op: %d data: %d\n", tmp->operation, tmp->data);
            switch (tmp->operation) {
                case 0:
                    insert_item(hm, tmp->data);
                    break;
                case 1:
                    print_list(hm->buckets[tmp->data], tmp->data);
                    break;
                case 2:
                    if (remove_item(hm, tmp->data))
                        printf("%d doesn't exist in hashmap\n", tmp->data);
                    break;
                case 3:
                    print_hashmap(hm);
                    break;
            }
            tmp->read = true;
        }
    }

    return 0;
}