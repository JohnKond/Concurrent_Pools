#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"
#include "queue.h"
#include "tree.h"
#define TRUE 1


/*
author: csd3964,
Giannis Kontogiorgakis
*/


int N;
int M;
extern struct treeNode *root;
struct queue *cat[];
size_t array_size;

pthread_barrier_t phase_1_barrier_end;
pthread_barrier_t phase_2_barrier_start;
pthread_barrier_t phase_2_barrier_end;
pthread_barrier_t phase_3_barrier_start;
pthread_barrier_t phase_3_barrier_end;
pthread_barrier_t phase_4_barrier_start;
pthread_barrier_t phase_4_barrier_end;


void *publisher_threads(void *ptr){
    int thread = (int)ptr;
    int i;
    
    for (i=0; i<N; i++){
       list_Insert(thread+i*M);
    }
    pthread_barrier_wait(&phase_1_barrier_end); 
    if(thread == 0){
        if(list_Count() == 0){
            list_Print();
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
        if(list_Keysum() == 0){
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_barrier_wait(&phase_2_barrier_start);
   
    if (thread < N){
        for (i=0; i<N; i++){
            list_Delete(thread+(i*M));
            queue_Insert(cat[thread%(N/4)],(thread+i*M));
            list_Delete(thread+(i*M)+N);
            queue_Insert(cat[thread%(N/4)],(thread+(i*M)+N));
        } 
    }

    pthread_barrier_wait(&phase_2_barrier_end);
   
    if(thread == 0){
        if(queue_Count(0) == 0){
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
        if (queue_Keysum() == 0){
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_barrier_wait(&phase_3_barrier_start);

    if (thread >= N && thread <= 2*N-1){
        for (i=0; i<N; i++){
            int pid = queue_Delete(cat[thread%(N/4)]);
            BST_Insert(pid);
        }
    }
    pthread_barrier_wait(&phase_3_barrier_end);
    
    if (thread == N){
        if (BST_count(0) == 0){
            inorder();
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
        if(queue_Count(1) == 0){
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_barrier_wait(&phase_4_barrier_start);

    if(thread < N ){    
        for (i=0; i<N; i++){
            if (BST_Search(thread+(i*M))){
                BST_Delete(thread+(i*M));
                queue_Insert(cat[thread%(N/4)],(thread+i*M));
            }
            if (BST_Search(thread+(i*M)+N)){
                BST_Delete(thread+(i*M));
                queue_Insert(cat[thread%(N/4)],(thread+i*M+N));
            }
        }
        
    }   

    pthread_barrier_wait(&phase_4_barrier_end);

    if (thread == 0){
        inorder();
        if (BST_count(1) == 0){
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
        if(queue_Count(1) == 0){
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
        if (queue_Keysum() == 0){
            printf("Exiting..\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

int main(int argc, char **argv){
    int i,j;


    N = atoi(argv[1]);
    M = 2*N;

    array_size = N/4;
    for (i=0; i<N/4; i++)
    cat[i] = malloc(array_size * sizeof(struct queue));
    pthread_t publisher[M];    
    pthread_barrier_init (&phase_1_barrier_end, NULL, M);
    pthread_barrier_init (&phase_2_barrier_start, NULL, M);
    pthread_barrier_init (&phase_2_barrier_end, NULL, M);
    pthread_barrier_init (&phase_3_barrier_start, NULL, M);
    pthread_barrier_init (&phase_3_barrier_end, NULL, M);
    pthread_barrier_init (&phase_4_barrier_start, NULL, M);
    pthread_barrier_init (&phase_4_barrier_end, NULL, M);
    


    LL_initialize();
    queue_initialize();
    for (j=0; j<M; j++){
        pthread_create(&publisher[j],NULL,publisher_threads,(void *)j);
    }


    for (j=0; j<M; j++){
        pthread_join(publisher[j], NULL);
    }

   
    pthread_barrier_destroy(&phase_1_barrier_end);
    pthread_barrier_destroy(&phase_2_barrier_start);
    pthread_barrier_destroy(&phase_2_barrier_end);
    pthread_barrier_destroy(&phase_3_barrier_start);
    pthread_barrier_destroy(&phase_3_barrier_end);
    pthread_barrier_destroy(&phase_4_barrier_start);
    pthread_barrier_destroy(&phase_4_barrier_end);

    
    return 0;
}



