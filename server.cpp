#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "lockhashmap.c"

struct sys_data {
    int operation;  // 0 insert  1 print a bucket list  2 delete
    int data;
};

int main(int argc, char const *argv[]) {
    if (argc != 2)
        printf("Usage: ./server <number of buckets>\n");
    HM *hm = alloc_hashmap(atoi(argv[1]));

    void *shm = (void *) 0;
    int shmid;
    struct sys_data *da;

    shmid = shmget((key_t) 8891, sizeof(struct sys_data), 0666 | IPC_CREAT);
    if (shmid == -1) {
        printf("shmget error\n");
        exit(-1);
    } else {
        printf("server shmid=%d\n", shmid);
    }
    shm = shmat(shmid, (void *) 0, 0);
    if (shm == (void *) (-1)) {
        printf("shmat error\n");
        exit(-1);
    }
    da = (sys_data *) shm;
    printf("shmat start\n");
    while (1) {
        sleep(3);
        printf("op: %d data: %d\n", da->operation, da->data);
        switch (da->operation) {
            case 0:
                insert_item(hm, da->data);
                break;
            case 1:
                print_list(hm->buckets[da->data], da->data);
                break;
            case 2:
                if (remove_item(hm, da->data))
                    printf("%d doesn't exist in hashmap\n", da->data);
                break;
        }
    }

    return 0;
}