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

int initdone;
int mainThreadId;
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

void freeThreadDS(int kthread){
    if(!headThread[kthread]){
        return;
    }
    thread_info* currThread=headThread[kthread];
    thread_info* nextThread;
    do{
        nextThread=currThread->next;
        free(currThread->stack);
        free(currThread->returnValue);
        free(currThread);
        currThread=nextThread;
    }while(currThread!=headThread[kthread]);

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
    printf("In signalHandler1\n");
}
void signalHandler2(int signal){
    printf("In signalHandler1\n");
}
void signalHandler3(int signal){
    printf("In signalHandler1\n");
}

int scheduler(void* arg){
    int kthreadNumber=*(int*)arg;
    if(kthreadNumber==0){
        signal(SIGALRM,signalHandler1);
    }
    else if(kthreadNumber==1){
        signal(SIGALRM,signalHandler2);
    }
    else{
        signal(SIGALRM,signalHandler3);
    }
    int mainThreadKilled=(kill(mainThreadId,0));
    while(!mainThreadId || headThread[kthreadNumber]){
        printf("hii\n");
        thread_info* nextRunnableThread=NULL;

        while(__sync_lock_test_and_set(&linkedListlock[kthreadNumber],1))
            ;
        if(headThread[kthreadNumber]){
            thread_info* currThread=headThread[kthreadNumber];

            do{
                if(currThread->state==RUNNABLE){
                    nextRunnableThread=currThread;
                    break;
                }
                currThread=currThread->next;
            }while(currThread!=headThread[kthreadNumber]);
        }

        __sync_lock_release(&linkedListlock[kthreadNumber]);
        printf("In scheduler %d\n",kthreadNumber);
        if(!nextRunnableThread && mainThreadKilled){
            printf("Killed %d\n",kthreadNumber);
            break;
        }
        else{
            mainThreadKilled=kill(mainThreadId,0);
            setTimer(0,TIMER_TIME);
            printf("Next runnable %d is %d\n",nextRunnableThread->threadId,kthreadNumber);
            swapcontext(&schedulerContext[kthreadNumber],&(nextRunnableThread->context));
            clearTimer();
        }
    }
    freeThreadDS(kthreadNumber);
    return 0;
}

int initThreadDS(){
    threadId=0;
    mutexId=0;
    mainThreadId=(int)gettid();

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

int thread_create(mythread_t* thread,void(*function)(void*),void* arg){
    if(!initdone){
        if(initThreadDS()==-1){
            return -1;
        }
        initdone=1;
    }

    thread_info* nn=newThread();
    

    if(!nn){
        return -1;
    }

    makecontext(&nn->context,(void(*)())function,1,arg);
    *thread=nn->threadId;
    int kid=kthreadId(nn->threadId);
    while(__sync_lock_test_and_set(&linkedListlock[kid],1))
        ;
    
    if(!headThread[kid]){
        nn->prev=nn->next=nn;
        headThread[kid]=nn;
    }
    else{
        nn->prev=headThread[kid]->prev;
        nn->next=headThread[kid];
        headThread[kid]->prev->next=nn;
        headThread[kid]->prev=nn;
    }

    __sync_lock_release(&linkedListlock[kid]);

    return 0;
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