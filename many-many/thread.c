#define _GNU_SOURCE
#include<unistd.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<malloc.h>
#include<sched.h>
#include<signal.h>
#include<string.h>
#include<linux/futex.h>
#include<stdint.h>
#include<sys/time.h>
#include<sys/wait.h>
#include"thread.h"
#include"timer.h"

#ifndef THREAD_C
#define THREAD_C

int initdone=0;
int threadId;
int mutexId;
thread_info* headThread[KTHREAD];
mythread_mutex_info* headMutex[KTHREAD];
mythread_mutex_info* backMutex[KTHREAD];
ucontext_t schedulerContext[KTHREAD];
ucontext_t mainContext;

int threadIdlock;
int mutexIdlock;
int headThreadLock[KTHREAD];
int headMutexLock[KTHREAD];
int backMutexLock[KTHREAD];


void scheduler(void* arg){

}

void initThreadDS(){
    threadId=0;
    mutexId=0;
    
    for(int i=0;i<KTHREAD;i++){
        headThread[i]=NULL;
        headMutex[i]=NULL;
        backMutex[i]=NULL;
        if(getcontext(&schedulerContext[i])==-1){
            perror("Error : ");
            return -1;
        }
        void* stack=malloc(STACK_SIZE);

        if(!stack){
            perror("Error : ");
            return -1;
        }

        schedulerContext[i].uc_link=NULL;
        schedulerContext[i].uc_stack.ss_size=STACK_SIZE;
        schedulerContext[i].uc_stack.ss_sp=stack;
        
        makecontext()
    }

    return 0;
}

int thread_create(mythread_t* thread,void(*function)(void),void* arg){
    if(!initdone){
        if(initThreadDS()==-1){
            return -1;
        }
        initdone=1;
    }
}

int thread_join(mythread_t* thread,void** returnValue){

}

void thread_exit(void* returnValue){

}

void thread_mutex_init(mythread_mutex_t* mutex){

}

int thread_mutex_destroy(mythread_mutex_t* mutex){


}

int thread_lock(mythread_mutex_t* mutex){

}


int thread_unlock(mythread_mutex_t* mutex){

}


int thread_mutex_unlock(mythread_mutex_t* mutex){

}


int thread_cancel(mythread_t* thread){

}

int thread_kill(mythread_t* thread,int signal){

}


#endif