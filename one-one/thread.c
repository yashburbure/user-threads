#define _GNU_SOURCE  
#include"thread.h"

#include<stdlib.h>
#include<unistd.h>
#include<sched.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<linux/futex.h>
#include<sys/syscall.h>



#ifndef thread_c
#define thread_c


#define NUMBEROFTHREAD 100

threadInfo threads[100];
int threadInitialize=0;
int futexIndex=0;

void initThreads(){
    for(int i=0;i<NUMBEROFTHREAD;i++){
        threads[i].threadId=-1;
        threads[i].returnValue=0;
        threads[i].stackPointer=0;
        threads[i].state=READY;
    }
}

int getReadyThread(){
    for(int i=0;i<NUMBEROFTHREAD;i++){
        if(threads[i].state==READY){
            return i;
        }
    }
    return -1;
}


int getThreadIndexByTid(){
    for(int i=0;i<NUMBEROFTHREAD;i++){
        if(threads[i].threadId==(int)gettid()){
            return i;
        }
    }
    return -1;
}


int thread_create(mythread_t* thread,int (*function)(void*),void* arg){
    if(!threadInitialize){
        initThreads();
        threadInitialize=1;
    }
    int threadIndex=getReadyThread();
    if(threadIndex==-1){
        fprintf(stderr,"Not Enough space for new Thread\n");
        return -1;
    }

    threads[threadIndex].stackPointer=malloc(STACKSIZE);
    if(threads[threadIndex].stackPointer==0){
        perror("Error : ");
        return -1;
    }
    threads[threadIndex].state=RUNNING;
    threads[threadIndex].threadId=
        clone(function,(void*)(threads[threadIndex].stackPointer+STACKSIZE),CLONE_VM,arg);

    if(threads[threadIndex].threadId==-1){
        perror("");
        return -1;
    }

    thread->threadId=threads[threadIndex].threadId;
    thread->threadIndex=threadIndex;
    return threads->threadId;


}
int thread_join(mythread_t* thread,void** returnValue){

    while(threads[thread->threadIndex].state!=TERMINATED){
        syscall(SYS_futex,&futexIndex,FUTEX_WAIT,threads[thread->threadIndex].state,NULL,NULL,0);
    }

    *returnValue=threads[thread->threadIndex].returnValue;

    free(threads[thread->threadIndex].stackPointer);
    threads[thread->threadIndex].stackPointer=0;
    threads[thread->threadIndex].state=READY;
    threads[thread->threadIndex].returnValue=0;
    threads[thread->threadIndex].threadId=-1;

    return 0;
}
void thread_exit(void* returnValue){
    char *message=0;
    if(returnValue!=0){
        message=(char*)malloc(strlen((char*)returnValue));
        strcpy(message,returnValue);
    }
    int threadIndex=getThreadIndexByTid();
    if(threadIndex==-1){
        exit(1);
    }
    threads[threadIndex].returnValue=(void*)message;
    threads[threadIndex].state=TERMINATED;
    syscall(SYS_futex,&futexIndex,FUTEX_WAKE,threads[threadIndex].state,NULL,NULL,0);
    exit(0);
}




#endif