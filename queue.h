#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

extern  int N;
extern  int M;
extern struct queue *cat[];
// extern size_t array_size; 

struct queue {
    struct queueNode *Head;
    struct queueNode *Tail;
    pthread_mutex_t headLock;
    pthread_mutex_t tailLock;
};

struct queueNode {
    int postID;
    struct queueNode *next;
};

// struct queue *cat[array_size];

void queue_initialize(){
    int i=0;
    for (i=0; i<N/4; i++){
        cat[i]->Head = NULL;
        cat[i]->Tail = NULL;
    }
}

int queue_Insert(struct queue *node,int pid){

    struct queueNode *new,*curr;
    new = (struct queueNode *)malloc(sizeof(struct queueNode));
    new->postID = pid;

    if (node->Head == NULL) {
        node->Head = new;
        node->Tail = new;
        return 1;
    }

    pthread_mutex_lock(&node->tailLock);
    node->Tail->next = new;
    node->Tail = new;
    new->next = NULL;
    pthread_mutex_unlock(&node->tailLock);

    return 1;
}


int queue_Delete(struct queue *node){
    int i,pid;
    struct queueNode *curr,*temp;

    pthread_mutex_lock(&node->headLock);
    if (node->Head->next == NULL) node->Head == NULL;
    else {
        pid = node->Head->postID;
        // printf("Delete from queue pid: %d\n",pid);
        node->Head = node->Head->next;
    }
    pthread_mutex_unlock(&node->headLock);    
    return pid;
}

void queue_Print(){
    struct queueNode *curr;
    int i=0;
    for (i=0; i<N/4; i++){
        curr = cat[i]->Head; 
        printf("This is category[%d]\n",i);
        while (curr){
            printf("%d\n",curr->postID);
            curr = curr->next;
        }
        printf("--------------------\n");
    }
}

int queue_Keysum(){
    struct queueNode *curr;
    int i,sum=0,expected_keysum=2*N*N*N*N-N*N;

    for (i=0; i<N/4; i++){
        curr = cat[i]->Head;
        while(curr){
            sum += curr->postID;
            curr = curr->next;
        }
    }
    printf("Total keysum check counted(expected: %d, found: %d\n",expected_keysum,sum);
    if (sum != expected_keysum) return 0;
    return 1;
}

int queue_Count(int delete_flag){
    struct queueNode *curr;
    int i=0,counter,expected_count;

    //if delete_flag == 1 , we want to count the queue after queue_delete.

    if (delete_flag) expected_count = 4*N;
    else expected_count = 8*N;

    for (i=0; i<N/4; i++){
        curr = cat[i]->Head; 
        counter = 0;
        printf("Category[%d] queue's total size counted: ",i);
        while (curr){
            counter++;
            curr = curr->next;
        }

        printf("(expected: %d, found: %d).\n",expected_count,counter);
    
        if (counter != expected_count) return 0;
    }
    return 1;
}