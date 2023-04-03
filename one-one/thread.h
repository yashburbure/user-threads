#ifndef thread_h
#define thread_h

#define STACKSIZE 4096

#define READY 0
#define RUNNING 1
#define TERMINATED 2


typedef struct mythread_t{
    int threadId;
    int threadIndex;
}mythread_t;

typedef struct threadInfo{
    int threadId;
    void* returnValue;
    void* stackPointer;
    int state;
    int futexIndex;
}threadInfo;



int thread_create(mythread_t*,int(*function)(void*),void*);
int thread_join(mythread_t*,void**);
void thread_exit(void*);

#endif