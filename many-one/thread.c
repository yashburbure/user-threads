#define _GNU_SOURCE
#include<unistd.h>
#include<sys/types.h>
#include<malloc.h>
#include<sched.h>
#include<signal.h>
#include<sys/time.h>
#include"thread.h"
#include"spinlock.h"
#include"timer.h"


#ifndef THREAD_C
#define THREAD_C

int initDone=0;
thread_info* headThread;
int mainThreadId;
int threadId[1000];
spinlock linkedListLock;



ucontext_t schedulerContext;


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

    threadId[nn->threadId]=1;

    if(nn->threadId==-1){
        fprintf(stderr,"All thread are being used\n");
        free(nn);
        return NULL;
    }

    nn->stack=malloc(STACK_SIZE);
    
    if(nn->stack==NULL){
        perror("Error : ");
        free(nn);
        return NULL;
    }

    nn->context.uc_stack.ss_size=STACK_SIZE;
    nn->context.uc_stack.ss_sp=nn->stack;
    nn->context.uc_link=&schedulerContext;

    nn->state=RUNNABLE;
    nn->next=nn->prev=nn->returnValue=nn->stack=NULL;
}
void signalHandler(int signal){
    printf("Signal Handled\n");
    thread_info* currThread=headThread;
    headThread=headThread->next;
    swapcontext(&(currThread->context),&(schedulerContext));
}


int scheduler(void* arg){
    signal(SIGALRM,signalHandler);
    printf("Scheduler started\n");
    int mainThreadKilled=(kill(mainThreadId,0)==0);
    while(!mainThreadId || headThread){
        thread_info* currThread=headThread;
        int runnableThread=0;
        if(headThread){
            acquire(&linkedListLock);
            do{
                if(currThread->state==RUNNABLE){
                    runnableThread++;
                }
                currThread=currThread->next;
            }while(currThread!=headThread);

            release(&linkedListLock);
        }
        if(mainThreadKilled && runnableThread==0){
            break;
        }
        else if(mainThreadKilled && runnableThread){

        }
        else{
            mainThreadKilled=(kill(mainThreadId,0)==0);
        }

        getcontext(&schedulerContext);
        if(clearTimer(0,TIMER_TIME)==-1){
            perror("Error : ");
        }

        if(headThread){
            while(linkedListLock.isLocked)
            ;
        
            acquire(&linkedListLock);

            currThread=headThread;



            thread_info* currThread=headThread;
            if(currThread){
                do{
                    if(currThread->state==RUNNABLE){
                        break;
                    }
                        currThread=currThread->next;
                }while(currThread!=headThread);
            }
            if(currThread->state==RUNNABLE){
                headThread=currThread;
            }
            release(&linkedListLock);
            if(setTimer(0,TIMER_TIME)==-1){
                perror("Error : ");
            }
            swapcontext(&schedulerContext,&(headThread->context)); 

        }
    }
    printf("Scheduler Ended\n");
    return 0;
}



int initThreadDS(){
    initspinLock(&linkedListLock);
    mainThreadId=getpid();
    headThread=NULL;
    void* stack=malloc(STACK_SIZE);
    if(!stack){
        perror("Error : ");
        return 1;
    }
    if(clone(&scheduler,stack+STACK_SIZE,CLONE_VM,0)==-1){
        perror("Error : ");
        return 1;
    }
    return 0;
}



int thread_create(mythread_t*,void(*function)(void),void*){
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

    while(linkedListLock.isLocked)
        ;

    acquire(&linkedListLock);

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

    thread_info* currThread=headThread;

    do{
        currThread=currThread->next;
    }while(currThread!=headThread);
    release(&linkedListLock);


    return 0;
}
int thread_join(mythread_t*,void**){
    while(linkedListLock.isLocked)
        ;
    // acquire(&linkedListLock);



    // release(&linkedListLock);
}
void thread_exit(void*){
    clearTimer();
    headThread->state=TERMINATED;
    // printf("%d\n",headThread->threadId);
    thread_info* currThread=headThread;

    do{
        // printf("STATE %d\n",currThread->state);
        currThread=currThread->next;
    }while(currThread!=headThread);

    swapcontext(&(headThread->context),&(schedulerContext));
}


#endif