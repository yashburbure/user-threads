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

int initDone=0;
thread_info* headThread;
int mainThreadId;
int threadId[1000];
int linkedListLock;



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

    nn->spinlock=nn->sleeplock=0;

    return nn;
}

void freeThreadDS(){
    if(!headThread){
        return;
    }
    thread_info* currThread=headThread;
    thread_info* nextThread;
    int ct=0;
    do{
        nextThread=currThread->next;
        free(currThread->stack);
        free(currThread->returnValue);
        free(currThread);
        currThread=nextThread;
    }while(currThread!=headThread);

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
        int runnableThread=0;
        if(headThread){

            while(linkedListLock)
                ;

            linkedListLock=1;

            thread_info* currThread=headThread;
            do{
                if(currThread->state==RUNNABLE){
                    runnableThread++;
                }
                currThread=currThread->next;
            }while(currThread!=headThread);

            linkedListLock=0;

        }
        if(mainThreadKilled && runnableThread==0){
            break;
        }
        else if(!mainThreadKilled){
            mainThreadKilled=(kill(mainThreadId,0)==0);
        }
        

        if(headThread){
            while(linkedListLock)
                ;
            
            linkedListLock=1;
            
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
            linkedListLock=0;

            if(setTimer(0,TIMER_TIME)==-1){
                perror("Error : ");
            }
            swapcontext(&schedulerContext,&(headThread->context)); 
            clearTimer();
        }
    }
    freeThreadDS();
    printf("Scheduler ended\n");
    return 0;
}



int initThreadDS(){
    linkedListLock=0;
    mainThreadId=gettid();
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

    while(linkedListLock)
        ;

    linkedListLock=1;

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

    linkedListLock=0;


    return 0;
}
int thread_join(mythread_t* thread,void** returnValue){
    

    while(linkedListLock)
        ;

    linkedListLock=1;

    thread_info* foundThread=NULL;
    thread_info* currThread=headThread;

    do{
        if(currThread->threadId==*thread){
            foundThread=currThread;
            break;
        }
        currThread=currThread->next;
    }while(currThread!=headThread);

    linkedListLock=0;

    if(!foundThread){
        printf("No such Thread\n");
        return 1;
    }
    while(foundThread->state!=TERMINATED){
        syscall(SYS_futex,&(foundThread->threadId),FUTEX_WAIT,foundThread->state,NULL,NULL,0);
    }

    while(linkedListLock)
        ;
    linkedListLock=1;

    if(currThread->next==currThread){
        headThread=NULL;

    }
    else{
        currThread->prev->next=currThread->next;
        currThread->next->prev=currThread->prev;
    }
    free(currThread->stack);
    free(currThread); 

    *returnValue=foundThread->returnValue;

    linkedListLock=0;
    return 0;
}

void thread_exit(void* returnValue){
    clearTimer();
    headThread->state=TERMINATED;
    if(returnValue){
         char* message=(char*)malloc(sizeof(returnValue));
        strcpy(message,(char*)returnValue);
        headThread->returnValue=message;
    }
    syscall(SYS_futex,&(headThread->threadId),FUTEX_WAKE,headThread->state,NULL,NULL,0);
    swapcontext(&(headThread->context),&(schedulerContext));
}

int thread_lock(mythread_t* thread){
    while(linkedListLock)
        ;
    linkedListLock=1;

    thread_info* currThread=headThread;
    thread_info* foundThread=NULL;

    do{
        if(currThread->threadId==*thread){
            foundThread=currThread;
            break;
        }
    }while(currThread!=headThread);

    if(!foundThread){
        return 1;
    }

    foundThread->spinlock=1;

    linkedListLock=0;

    

}

int thread_unlock(mythread_t*){


}

int thread_mutex_lock(mythread_t*){


}
int thread_mutex_unlock(mythread_t*){
    
}





#endif