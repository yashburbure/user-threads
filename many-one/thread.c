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
int mutexId;
mythread_mutex_info* headMutex=NULL;
mythread_mutex_info* backMutex=NULL;

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
    // printf("Signal Handled\n");
    // printf("Signal Handled %d\n",headThread->threadId);
    thread_info* currThread=headThread;
    headThread=headThread->next;
    swapcontext(&(currThread->context),&(schedulerContext));
}


int scheduler(void* arg){
    signal(SIGALRM,signalHandler);
    printf("Scheduler started\n");
    int mainThreadKilled=(kill(mainThreadId,0)==0);
    while(!mainThreadKilled || headThread){
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
            if(headThread->state==RUNNABLE)
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
    mutexId=0;
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



int thread_create(mythread_t* thread,void(*function)(void),void*){
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
    *thread=nn->threadId;
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
    linkedListLock=0;

    return 0;
}   
int thread_join(mythread_t* thread,void** returnValue){
    // thread_info* temp=headThread;
    // do{
    //     printf("%d\n",temp->threadId);
    //     temp=temp->next;
    // }while(temp!=headThread);



    while(linkedListLock)
        ;

    linkedListLock=1;

    thread_info* foundThread=NULL;
    thread_info* currThread=headThread;

    //ERROR HEADTHREAD IS NOT CHANGING

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
void thread_mutex_init(mythread_mutex_t* mutex){
    *mutex=mutexId++;
    mythread_mutex_info* nn=(mythread_mutex_info*)malloc(sizeof(mythread_mutex_info));
    nn->mutexId=*mutex;
    nn->next=NULL;
    nn->isLocked=0;
    if(!headMutex){
        headMutex=nn;
        backMutex=nn;
    }
    else{
        backMutex->next=nn;
        backMutex=nn;
    }

}

int thread_mutex_destroy(mythread_mutex_t* mutex){
    mythread_mutex_info* currMutex=headMutex;
    mythread_mutex_info* prevMutex=NULL;
    while(currMutex){
        if(currMutex->mutexId==*mutex){
            break;
        }
        prevMutex=currMutex;
        currMutex=currMutex->next;
    }
    if(!currMutex){
        return -1;
    }
    if(currMutex==headMutex){
        headMutex=currMutex->next;
    }
    else{
        prevMutex->next=currMutex->next;
    }
    currMutex->next=NULL;
    free(currMutex);
    return 0;
}

// void print(){
//     if(headThread){
//         printf("THREADS :: ");
//         thread_info* currThread=headThread;
//         do{
//             printf("%d:: ",currThread->threadId);
//             currThread=currThread->next;
//         }while(currThread!=headThread);
//         printf("\n");
//     }
//     mythread_mutex_info* currMutex=headMutex;
//     printf("MUTEX :: ");
//     if(!headMutex){
//         printf("No mutex\n");
//     }
//     while(currMutex){
//         printf("%d:: ",currMutex->mutexId);
//         currMutex=currMutex->next;
//     }
//     printf("\n");
// }

int thread_lock(mythread_mutex_t* mutex){
    mythread_mutex_info* currMutex=headMutex;
    while(currMutex){
        // printf("%d :: %d\n",currMutex->mutexId,*mutex);
        if(currMutex->mutexId==*mutex){
            break;
        }
        currMutex=currMutex->next;
    }
    if(!currMutex){
        printf("No such mutex\n");
        return -1;
    }
    //spinning for lock
    while(__sync_lock_test_and_set(&currMutex->isLocked,1)!=0){
        ;
        // printf("Waiting for lock\n");
    }
    // currMutex->isLocked=1;
    return 0;
}

int thread_unlock(mythread_mutex_t* mutex){
    mythread_mutex_info* currMutex=headMutex;
    while(currMutex){
        if(currMutex->mutexId==*mutex){
            break;
        }
        currMutex=currMutex->next;
    }
    if(!currMutex){
        return -1;
    }
    __sync_lock_release(&currMutex->isLocked);
    // currMutex->isLocked=0;
    return 0;
}

int thread_mutex_lock(mythread_mutex_t* mutex){
    mythread_mutex_info* currMutex=headMutex;
    while(currMutex){
        if(currMutex->mutexId==*mutex){
            break;
        }
        currMutex=currMutex->next;
    }
    if(!currMutex){
        return -1;
    }

    //spinning for lock
    while(currMutex->isLocked){
        syscall(SYS_futex,&(currMutex->mutexId),FUTEX_WAIT,currMutex->isLocked,NULL,NULL,0);
    }
    currMutex->isLocked=1;
    return 0;

}


int thread_mutex_unlock(mythread_mutex_t* mutex){
    mythread_mutex_info* currMutex=headMutex;
    mythread_mutex_info* temp=backMutex;
    while(currMutex){
        if(currMutex->mutexId==*mutex){
            break;
        }
        currMutex=currMutex->next;
    }
    if(!currMutex){
        return -1;
    }
    currMutex->isLocked=0;

    syscall(SYS_futex,&(currMutex->mutexId),FUTEX_WAKE,currMutex->isLocked,NULL,NULL,0);

    return 0;
}





#endif