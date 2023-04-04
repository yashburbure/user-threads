#include<ucontext.h>
#ifndef THREAD_H
#define THREAD_H


#define RUNNING 0
#define WAITING 1
#define TERMINATED 2

#define STACK_SIZE 2*1024*1024

typedef int mythread_t;


typedef struct thread_info{
    int threadId;
    void* stack;
    void* returnValue;
    int state;
    ucontext_t context;
    struct thread_info* prev;
    struct thread_info* next;
}thread_info;


int thread_create(mythread_t*,int(*function)(void*),void*);
int thread_join(mythread_t*,void**);
void thread_exit(void*);


#endif