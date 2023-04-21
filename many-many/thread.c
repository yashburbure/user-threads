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
int threadIdlock;
int mutexIdlock;

thread_info* headThread[KTHREAD];
mythread_mutex_info* headMutex[KTHREAD];
mythread_mutex_info* backMutex[KTHREAD];
ucontext_t schedulerContext[KTHREAD];

int linkedListlock[KTHREAD];
int headMutexLock[KTHREAD];
int backMutexLock[KTHREAD];

int threadNumber[KTHREAD];

int kthreadId(int threadid){
    return (threadid)%3;
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

    nn->threadId=threadId++;

    nn->stack=malloc(STACK_SIZE);
    
    if(nn->stack==NULL){
        threadId--;
        perror("Error : ");
        free(nn);
        return NULL;
    }

    nn->context.uc_stack.ss_size=STACK_SIZE;
    nn->context.uc_stack.ss_sp=nn->stack;
    nn->context.uc_link=&schedulerContext[kthreadId(nn->threadId)];

    nn->state=RUNNABLE;
    nn->next=nn->prev=nn->returnValue=nn->stack=NULL;

    return nn;
}


void signalHandler1(int signal){

}
void signalHandler2(int signal){
    
}
void signalHandler3(int signal){
    
}

int scheduler(void* arg){
    int kthreadNumber=*(int*)arg;
    printf("%d\n",kthreadNumber);



    return 0;
}

int initThreadDS(){
    threadId=0;
    mutexId=0;
    
    for(int i=0;i<KTHREAD;i++){
        headThread[i]=NULL;
        headMutex[i]=NULL;
        backMutex[i]=NULL;

        threadNumber[i]=i;
        void* stack=malloc(STACK_SIZE);
        if(!stack){
            perror("Error : ");
            return -1;
        }
        if(clone(&scheduler,stack+STACK_SIZE,CLONE_VM,(void*)(&threadNumber[i]))==-1){
            perror("Error : ");
            return -1;
        }
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

    // thread_info* nn=newThread();
    // if(!nn){
    //     return -1;
    // }
    
    // *thread=nn->threadId;
    // int kid=kthreadId(nn->threadId);
    // while(__sync_lock_test_and_set(&linkedListlock[kid],1))
    //     ;
    
    // nn->prev=headThread[kid]->prev;
    // nn->next=headThread;
    // headThread[kid]->prev->next=nn;
    // headThread[kid]->prev=nn;


    // __sync_lock_release(&linkedListlock[kid]);

    // return 0;
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