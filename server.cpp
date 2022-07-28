#include <iostream>

#include "chashmap.h"


int main(int argc, char **argv) {
    int n_buckets;
    if (argc == 2)
        n_buckets = atoi(argv[1]);
    HM* hm = alloc_hashmap(n_buckets);
}