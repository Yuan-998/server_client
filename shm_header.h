#ifndef SERVER_CLIENT_SHM_HEADER_H
#define SERVER_CLIENT_SHM_HEADER_H

#define SIZE 5

struct message {
    int operation;  // 0 insert  1 print a bucket list  2 delete 3 print hashmap
    int data;
    bool read = true;
};


int poll_server(struct message *me) {
    for (int i = 0; i < SIZE; ++i) {
        if ((me + i)->read == false)
            return i;
    }
    return -1;
}

int poll_client(struct message *me) {
    for (int i = 0; i < SIZE; ++i) {
        if ((me + i)->read == true)
            return i;
    }
    return -1;
}

#endif //SERVER_CLIENT_SHM_HEADER_H
