#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIZE 5

struct message {
    int operation;  // 0 insert  1 print a bucket list  2 delete
    int data;
    bool read;
};

int poll(struct message *me) {
    for (int i = 0; i < SIZE; ++i) {
        if ((me + i)->read == true)
            return i;
    }
    return -1;
}

int main(int argc, char const *argv[]) {
    void *shm = (void *) 0;
    int shmid;
    struct message *me;

    shmid = shmget((key_t) 6666, sizeof(struct message) * SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        printf("shmget error\n");
        exit(-1);
    } else {
        printf("client shmid=%d\n", shmid);
    }

    shm = shmat(shmid, (void *) 0, 0);
    if (shm == (void *) (-1)) {
        printf("shmat error\n");
        exit(-1);
    }
    me = (message *) shm;
    while (1) {
        int index = poll(me);
        if (index != -1) {
            printf("input operation and data: \n");
            scanf("%d %d", &(me + index)->operation, &(me + index)->data);
            (me + index)->read = false;
        }
    }

    return 0;
}