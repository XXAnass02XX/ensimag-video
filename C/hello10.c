#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "thread_args.h"


pthread_mutex_t mutex;
pthread_cond_t condition;
int compteur;

struct Thread_args* args;

void* incrementer(void* arg) {
    struct Thread_args* args = (struct Thread_args*)arg;
    pthread_mutex_lock(&args->mutex);
    args->compteur++;
    if (args->compteur == 10) {
        pthread_cond_broadcast(&args->condition);
    }
    pthread_mutex_unlock(&args->mutex);
}

void* wait_10(void* arg){
    struct Thread_args* args = (struct Thread_args*)arg;
    pthread_mutex_lock(&args->mutex);
    if(args->compteur < 10){
        pthread_cond_wait(&args->condition, &args->mutex);
    }
    pthread_mutex_unlock(&args->mutex);
}

void *print_hello(void* arg){
    printf("Hello World ! \n");
    incrementer(arg);
}

void *print_done(void* arg){
    wait_10(arg);
    printf("Done ! \n");
}


int main(){
    args = malloc(sizeof(struct Thread_args)); 
    pthread_t threads[11];
    args->compteur = 0;
    pthread_mutex_init(&(args->mutex), NULL);
    pthread_cond_init(&(args->condition), NULL);
    compteur = 0;
    int i;
    for(i = 0; i < 10; i++){
        pthread_create(&threads[i], NULL, print_hello, args);
    }
    pthread_create(&threads[10], NULL, print_done, args);
    for(i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }
    free(args);
    return 0;
}