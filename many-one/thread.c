#define _GNU_SOURCE
#include<unistd.h>
#include<sys/types.h>
#include<malloc.h>
#include<sched.h>
#include<signal.h>
#include"thread.h"

#ifndef THREAD_C
#define THREAD_C

int initDone=0;
thread_info* headThread;
int mainThreadId;
int threadId[1000];

int getThreadId(){
    for(int i=0;i<1000;i++){
        if(threadId[i]==0){
            return i;
        }
    }
    return -1;
}


thread_info* newThread(){
    thread_info* nn=(thread_info*)malloc(sizeof(thread_info));
    if(!nn){
        perror("Error : ");
        return NULL;
    }

    if(getcontext(&nn->context)==-1){
        perror("Error : ");
        free(nn);
        return NULL;
    }

    nn->threadId=getThreadId();

    if(nn->threadId==-1){
        fprintf(stderr,"All thread are begin used\n");
        free(nn);
        return NULL;
    }

    nn->stack=malloc(STACK_SIZE);
    
    if(nn->stack==NULL){
        perror("Error : ");
        free(nn);
        return NULL;
    }

    nn->state=WAITING;
    nn->next=nn->prev=nn->returnValue=nn->stack=NULL;
}


int scheduler(void* arg){
    mainThreadId=gettid();
    while(kill(mainThreadId,0)==0 || headThread){

    }
    return 0;
}


int initThreadDS(){
    headThread=NULL;
    void* stack=malloc(STACK_SIZE);
    if(clone(&scheduler,stack+STACK_SIZE,CLONE_VM,0)==-1){
        perror("Error : ");
        return 1;
    }
    return 0;
}



int thread_create(mythread_t*,void(*function)(void*),void*){
    if(!initDone){
        if(initThreadDS()==1){
            return 1;
        }
        initDone=1;
    }

    thread_info* nn=newThread();
    if(!nn){
        return 1;
    }
    makecontext(&nn->context,function,0);
    if(!headThread){
        nn->prev=nn->next=nn;
        headThread=nn;
    }
    else{
        nn->prev=headThread->prev;
        nn->next=headThread;
        headThread->prev->next=nn;
        headThread->prev=nn;
    }

    return 0;
}
int thread_join(mythread_t*,void**){

}
void thread_exit(void*){

}


#endif