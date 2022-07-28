#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct sys_data {
    int operation;  // 0 insert  1 print a bucket list  2 delete
    int data;
};

int main(int argc, char const *argv[]) {
    void *shm = (void *) 0;
    int shmid;
    struct sys_data *da;
    float ftemp = 0.0, fhumi = 0.0;

    shmid = shmget((key_t) 8891, sizeof(struct sys_data), 0666 | IPC_CREAT);
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
    da = (sys_data *) shm;
    while (1) {
        printf("input operation and data: \n");
        scanf("%d %d", &da->operation, &da->data);
    }

    return 0;
}