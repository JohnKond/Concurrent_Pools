
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

extern int N;
extern int M;

struct LLNode {
    int postID;
    int marked;
    pthread_mutex_t lock;
    struct LLNode *next;
};

struct SinglyLinkedList {
    struct LLNode *head;
    struct LLNode *tail;
};

struct SinglyLinkedList link;



void list_Print(){
    struct LLNode *p;
    p = link.head->next;
    while(p != link.tail){
        printf("pid = %d\n",p->postID);
        p = p->next;
    }
}

int validate(struct LLNode *pred,struct LLNode *curr){
    if((pred->marked == 0) && (curr->marked == 0)){
        if (pred->next == curr) return 1;
    }
    return 0;
}

int list_Delete(int pid){
    int result = 0;
    struct LLNode *pred,*temp,*curr,*del;
 
    if (list_Search(pid) == 0) {
        // printf("Pid %d not in the list!\n",pid);
        return 0;
    }

    pred = link.head;
    curr = pred->next;

    if(pred->postID == pid) {
        temp = curr;
        curr = pred;
    }
    else{
        while(curr->postID < pid){
            pred = curr;
            curr = pred->next;
        }
    }

    if(pred && pred!=curr) pthread_mutex_lock(&pred->lock);
    if(curr) pthread_mutex_lock(&curr->lock);

    if(validate(pred,curr)){
        if (curr->postID == pid){
            curr->marked = 1;
            del = curr;
            if (curr == pred) link.head = temp;
            else if (curr->next == NULL) pred->next = NULL;
            else pred->next = curr->next;
            // printf("Deleted pid %d\n",pid);
            result = 1;
        }else{
            result = 0;
        }   
    }else result = 0;

    if(pred && pred!=curr) pthread_mutex_unlock(&pred->lock);
    if(curr) pthread_mutex_unlock(&curr->lock);
    return result;
}

void LL_initialize(){
    // link = (struct SinglyLinkedList *)malloc(sizeof(struct SinglyLinkedList));
    
    // struct LLNode *new1,*new2;
    // new1 = (struct SinglyLinkedList *)malloc(sizeof(struct SinglyLinkedList));
    // new2 = (struct SinglyLinkedList *)malloc(sizeof(struct SinglyLinkedList));
    // link = NULL;
    link.head = (struct LLNode *)malloc(sizeof(struct LLNode ));
    link.tail = (struct LLNode *)malloc(sizeof(struct LLNode ));
    link.head->next = link.tail;
}


int list_Insert(int pid){
    struct LLNode *pred,*curr;
    int result,return_flag = 0;

    while (1) {
        pred = link.head;
        curr = pred->next;

        while (curr != link.tail &&curr->postID < pid) {
            pred = curr;
            curr = curr->next;
        }

        pthread_mutex_lock(&pred->lock);
        pthread_mutex_lock(&curr->lock);    
        if (validate(pred, curr)) {
            if (pid == curr->postID) {
                result = 0; 
                return_flag = 1;
            }
            else {
                struct LLNode *node = (struct LLNode*)malloc(sizeof(struct LLNode));
                node->next = curr; 
                node->postID = pid;
                pred->next = node;
                result = 1; 
                return_flag = 1;
            }
        }
        pthread_mutex_unlock(&pred->lock);
        pthread_mutex_unlock(&curr->lock); 
        if (return_flag) return result;
    }
}

/*
int list_Insert(int pid){
    struct LLNode *new = (struct LLNode *)malloc(sizeof(struct LLNode));
    struct LLNode *pred,*curr,*dummy; 
    int result,return_flag = 0;
    new->postID = pid;

    // printf("Gonna insert pid %d\n",pid);

    if(link->head == NULL){
        link->head = new;
        pthread_mutex_lock(&link->head->lock);
        new->next = NULL;
        pred = new;
        pthread_mutex_unlock(&link->head->lock);
        return 1;
    }
   
    
    pred = link->head;
    curr = pred->next;


    if(link->head->postID > pid){
        if(curr){
            curr = pred;
            pthread_mutex_lock(&curr->lock);
        }
        if (validate(pred,curr) == 1){
            link->head = new;
            new->next = curr;
        }
        if(curr) pthread_mutex_unlock(&curr->lock);
        return 1;         
    }

        while(curr && curr->postID < pid){
            pred = curr;
            curr = pred->next;
        }


        if(pred) pthread_mutex_lock(&pred->lock);
        if(curr) pthread_mutex_lock(&curr->lock);
        
            new->next = curr;
            pred->next = new;                   
        
        if(pred) pthread_mutex_unlock(&pred->lock);
        if(curr) pthread_mutex_unlock(&curr->lock);
    
    return 1;
}
*/

int list_Keysum(){
    int keysum=0;
    int expected_keysum =(2*N*N*N*N)-(N*N);
    struct LLNode *p;
    
    p=link.head->next;
    while(p != link.tail){
        keysum += p->postID;
        p = p->next;
    }

    printf("Total keysum counter (expected: %d, found: %d\n",expected_keysum,keysum);
    if (keysum != expected_keysum) return 0;
}


int list_Count(){
    int sum = 0;
    int expected_sum = (2*N*N);
    
    struct LLNode *p;
    p = link.head->next;
    while(p != link.tail){
        sum++;
        p = p->next;
    }

    printf("Total list size counted (expected: %d, found: %d\n",expected_sum,sum);
    if(sum != expected_sum) return 0;
    return 1;
}



int list_Search(int pid){
    struct LLNode *curr,*pred;
    int result = 0;

    pred = link.head->next;
    curr = pred->next;

    if (pred->postID == pid) curr = pred;
    else  
        while(curr != link.tail && curr->postID < pid){ 
            pred = curr;
            curr = curr->next;
        }
    
    if (curr == NULL) return 0;
    
    if(pred && pred!=curr) pthread_mutex_lock(&pred->lock);
    if(curr) pthread_mutex_lock(&curr->lock);

    if(validate(pred,curr) == 1){
        if(curr->postID == pid) result = 1; 
    } else result = 0;
    if (curr) pthread_mutex_unlock(&curr->lock);
    if (pred && pred!=curr) pthread_mutex_unlock(&pred->lock);
    return result;
}



