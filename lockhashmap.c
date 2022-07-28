#include "chashmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int get_list_length(List *list) {
    Node_HM *curr = list->sentinel;
    int length = 0;
    while (curr != NULL) {
        length++;
        curr = curr->m_next;
    }
    return length;
}

void free_list(List *list) {
    Node_HM *curr = list->sentinel;
    Node_HM *next = list->sentinel->m_next;
    int length = get_list_length(list);
    for (int i = 0; i < length; ++i) {
        free(curr);
        curr = next;
        if (i != length - 1)
            next = next->m_next;
    }
}

HM *alloc_hashmap(size_t n_buckets) {
    HM *hm = (HM *) malloc(sizeof(HM));
    hm->n_buckets = n_buckets;
    hm->buckets = (List **) malloc(sizeof(List) * n_buckets);
    for (size_t i = 0; i < n_buckets; ++i) {
        hm->buckets[i] = (List *) malloc(sizeof(List));
        hm->buckets[i]->sentinel = (Node_HM *) malloc(sizeof(Node_HM));
        pthread_rwlock_init(&hm->buckets[i]->sentinel->rwlock, NULL);
    }
    return hm;
}

int hash(long val, size_t n_buckets) {
    return val % n_buckets;
}


void free_hashmap(HM *hm) {
    for (size_t i = 0; i < hm->n_buckets; ++i) {
        free_list(hm->buckets[i]);
    }
    free(hm->buckets);
    free(hm);
}


int insert_item(HM *hm, long val) {
    int index = hash(val, hm->n_buckets);
    List *tmp = hm->buckets[0 + index];
    Node_HM *sentinel = tmp->sentinel;
    pthread_rwlock_rdlock(&sentinel->rwlock);
    Node_HM *node_next = sentinel->m_next;
    pthread_rwlock_unlock(&sentinel->rwlock);
    Node_HM *node = (Node_HM *) malloc(sizeof(Node_HM));
    if (node == NULL)
        return 1;
    pthread_rwlock_init(&node->rwlock, NULL);
    node->m_val = val;
    pthread_rwlock_wrlock(&sentinel->rwlock);
    sentinel->m_next = node;
    node->m_next = node_next;
    pthread_rwlock_unlock(&sentinel->rwlock);
    return 0;
}

int remove_item(HM *hm, long val) {
    if (lookup_item(hm, val))
        return 1;
    int index = hash(val, hm->n_buckets);

    List *tmp = hm->buckets[0 + index];
    pthread_rwlock_rdlock(&tmp->sentinel->rwlock);
    Node_HM *curr = tmp->sentinel;
    pthread_rwlock_unlock(&tmp->sentinel->rwlock);
    while (1) {
        pthread_rwlock_t tmp_lock = curr->rwlock;
        pthread_rwlock_rdlock(&tmp_lock);
        if (curr->m_next->m_val == val)
            break;
        curr = curr->m_next;
        pthread_rwlock_unlock(&tmp_lock);
    }
    pthread_rwlock_wrlock(&curr->rwlock);
    curr->m_next = curr->m_next->m_next;
    print_hashmap(hm);
    pthread_rwlock_unlock(&curr->rwlock);
    return 0;
}


int lookup_item(HM *hm, long val) {
    int index = hash(val, hm->n_buckets);
    List *tmp = hm->buckets[0 + index];
    Node_HM *curr = tmp->sentinel->m_next;
    int flag = 1;
    while (curr != NULL) {
        pthread_rwlock_rdlock(&curr->rwlock);
        if (curr->m_val == val) {
            flag = 0;
            break;
        }
        curr = curr->m_next;
        pthread_rwlock_unlock(&curr->rwlock);
    }
    return flag;
}

void print_list(List *list, int i_bucket) {
    Node_HM *tmp = list->sentinel->m_next;
    printf("Bucket %d ", i_bucket + 1);
    while (tmp != NULL) {
        printf("- %ld ", tmp->m_val);
        tmp = tmp->m_next;
    }
    printf("\n");
}

void print_hashmap(HM *hm) {
    for (size_t i = 0; i < hm->n_buckets; ++i) {
        print_list(hm->buckets[i], i);
    }
}