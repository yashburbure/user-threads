#include<ucontext.h>
#ifndef THREAD_H
#define THREAD_H


#define RUNNABLE 0
#define TERMINATED 1

#define STACK_SIZE 2*1024*1024

#define TIMER_TIME 1000


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


int thread_create(mythread_t*,void(*function)(void),void*);
int thread_join(mythread_t*,void**);
void thread_exit(void*);


#endif