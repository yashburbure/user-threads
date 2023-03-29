#define _GNU_SOURCE  
#include"thread.h"

#include<stdlib.h>
#include<unistd.h>
#include<sched.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>

#ifndef thread_c
#define thread_c


#define NUMBEROFTHREAD 100

mythread_t threads[NUMBEROFTHREAD];
int threadInitialize=0;

void initThreads(){
    for(int i=0;i<NUMBEROFTHREAD;i++){
        threads[i].threadId=-1;
        threads[i].returnValue=0;
        threads[i].stackPointer=0;
        threads[i].state=READY;
        threads[i].threadIndex=-1;
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

    threads[threadIndex].threadIndex=threadIndex;
    threads[threadIndex].stackPointer=malloc(STACKSIZE);
    if(threads[threadIndex].stackPointer==0){
        perror("Error : ");
        return -1;
    }
    thread->state=RUNNING;
    threads[threadIndex].threadId=
        clone(function,(void*)(threads[threadIndex].stackPointer+STACKSIZE),CLONE_VM,arg);

    if(threads[threadIndex].threadId==-1){
        perror("");
        return -1;
    }

    *thread=threads[threadIndex];
    return threads->threadId;
}
int thread_join(mythread_t* thread,void** returnValue){
    while(threads[thread->threadIndex].state!=TERMINATED)
        ;

    *returnValue=thread->returnValue;

    free(thread->stackPointer);
    thread->returnValue=0;
    thread->stackPointer=0;
    thread->state=READY;
    thread->threadId=-1;
    thread->threadIndex=-1;
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
    exit(0);
}




#endif