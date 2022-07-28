#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>//ipc
#include <sys/shm.h>
#include <unistd.h>

#include "lockhashmap.c"

struct sys_data {
    float data_rh;
    float data_t;
};

//基于课程http://edu.51cto.com/lesson/id-131448.html
//linux c之shm共享内存的使用例子
int main(int argc, char const *argv[]) {
    if (argc != 2)
        printf("Usage: ./server <number of buckets>\n");
    HM *hm = alloc_hashmap(atoi(argv[1]));
    insert_item(hm, 2);
    print_hashmap(hm);

    void *shm = (void *) 0;
    int shmid;
    struct sys_data *da;
    float ftemp = 0.0, fhumi = 0.0;
    //set share memory;
    //linxu可以使用ipcrm -m shmid 删除此共享内存。
    //创建一个共享内存对象
    shmid = shmget((key_t) 8891, sizeof(struct sys_data), 0666 | IPC_CREAT);
    if (shmid == -1) {
        printf("shmget error\n");
        exit(-1);
    } else {
        printf("server shmid=%d\n", shmid);
    }
    //把共享内存映射到调用进程的地址空间
    //挂载共享内存到进程中
    shm = shmat(shmid, (void *) 0, 0);
    if (shm == (void *) (-1)) {
        printf("shmat error\n");
        exit(-1);
    }
    da = (sys_data *) shm;
    printf("shmat start\n");
    while (1) {
        sleep(2);
        printf("temp=%.1f,humi=%.1f\n", da->data_t,
               da->data_rh);
    }

    return 0;
}