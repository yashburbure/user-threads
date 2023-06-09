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
#include"mutexlock.h"
#include"spinlock.h"
#include"sleeplock.h"


#ifndef THREAD_C
#define THREAD_C

int initDone;
thread_info* headThread=NULL;
int threadId=0;
int threadIdlock=0;
int listlock=0;

ucontext_t schedulerContext;


thread_info* newThread(void* arg){
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

    while(__sync_lock_test_and_set(&threadIdlock,1))
        ;

    nn->threadId=threadId++;

    __sync_lock_release(&threadIdlock);

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


void scheduler(){
    while(1){
        while(__sync_lock_test_and_set(&listlock,1))
            ;
    
        thread_info* currThread=headThread;

        if(currThread){
            do{
                if(currThread->state==RUNNABLE){
                    break;
                }
                currThread=currThread->next;
            }while(currThread!=headThread);

            if(currThread->state==RUNNABLE){
                headThread=currThread;
            }
        }

        __sync_lock_release(&listlock);

        if(headThread->state==RUNNABLE){
            if(setTimer(0,TIMER_TIME)==-1){
                perror("Error : ");
            }
            else{
                swapcontext(&schedulerContext,&(headThread->context)); 
            }
            itimerval timer;
            getitimer(ITIMER_REAL,&timer);
            if(timer.it_value.tv_usec>0){
                headThread->state=TERMINATED;
                headThread=headThread->next;
            }
            if(clearTimer()==-1){
                perror("Error : ");
            }
        }
    }
}

void signalHandler(int signal){
    thread_info* currThread=headThread;
    headThread=headThread->next;
    swapcontext(&currThread->context,&schedulerContext);
}


thread_info* headThreadGen(){
    thread_info* nn=(thread_info*)malloc(sizeof(thread_info));
    if(!nn){
        return NULL;
    }
    getcontext(&nn->context);
    nn->stack=NULL;
    nn->next=nn->prev=nn->returnValue=NULL;
    nn->state=RUNNABLE;
    while(__sync_lock_test_and_set(&threadIdlock,1))
        ;

    nn->threadId=threadId++;

    __sync_lock_release(&threadIdlock);

    return nn;
}




int thread_create(mythread_t* thread,void(*function)(void*),void* arg){
    if(!initDone){
        signal(SIGALRM,signalHandler);
        
        setTimer(0,TIMER_TIME);
        thread_info* nn=headThreadGen();

        if(!nn){
            return -1;
        }
        headThread=nn;
        headThread->next=headThread->prev=headThread;
        getcontext(&schedulerContext);
        schedulerContext.uc_link=&nn->context;
        schedulerContext.uc_stack.ss_size=STACK_SIZE;
        void* stack=malloc(STACK_SIZE);
        if(!stack){
            perror("Error : ");
            return -1;
        }
        schedulerContext.uc_stack.ss_sp=stack;
        
        makecontext(&schedulerContext,&scheduler,1,arg);
        initDone=1;
    }

    thread_info* nn=newThread(arg);
    if(!nn){
        perror("Error : ");
        return -1;
    }
    *thread=nn->threadId;
    
    makecontext(&nn->context,(void (*)())function,1,arg);

    while(__sync_lock_test_and_set(&listlock,1))
        ;

    nn->prev=headThread->prev;
    nn->next=headThread;
    headThread->prev->next=nn;
    headThread->prev=nn;

    __sync_lock_release(&listlock);


    return 0;
}

int thread_join(mythread_t* thread,void** returnValue){
    if(!headThread){
        fprintf(stderr,"No such thread\n");
        return -1;
    }
    thread_info* foundThread=NULL;
    thread_info* currThread=headThread;

    do{
        if(currThread->threadId==*thread){
            foundThread=currThread;
            break;
        }
        currThread=currThread->next;
    }while(currThread!=headThread);



    if(!foundThread){
        fprintf(stderr,"No such Thread\n");
        return -1;
    }
    while(foundThread->state!=TERMINATED){
        if(clearTimer()==-1){
            perror("Error : ");
        }
        thread_info* currThread=headThread;
        headThread=headThread->next;
        swapcontext(&currThread->context,&schedulerContext);
    }

    if(returnValue){
        *returnValue=foundThread->returnValue;
    }
    itimerval timer;
    getitimer(ITIMER_REAL,&timer);
    if(clearTimer()==-1){
        perror("Error : ");
    }

    if(foundThread==headThread){
        foundThread->next->prev=foundThread->prev;
        foundThread->prev->next=foundThread->next;

        headThread=headThread->next;

        free(foundThread->stack);
        free(foundThread);
    }
    else{
        foundThread->next->prev=foundThread->prev;
        foundThread->prev->next=foundThread->next;
        
        free(foundThread->stack);
        free(foundThread);
    }
    setitimer(ITIMER_REAL,&timer,NULL);


    return 0;
}

void thread_exit(void* returnValue){
    if(clearTimer()==-1){
        fprintf(stderr,"Failed to set Timer\n");
    }

    headThread->state=TERMINATED;
    if(returnValue){
        char* message=(char*)malloc(sizeof(returnValue));
        strcpy(message,(char*)returnValue);
        headThread->returnValue=message;
    }

    if(setTimer(0,TIMER_TIME)==-1){
        fprintf(stderr,"Failed to set Timer\n");
    }
    thread_info* currThread=headThread;
    headThread=headThread->next;
    swapcontext(&(currThread->context),&(schedulerContext));
}

int thread_mutex_init(mythread_mutex_t* mutex){
    return mutex_init_wrapper((int*)mutex);
}

int thread_mutex_destroy(mythread_mutex_t* mutex){
    return mutex_destroy_wrapper((int*)mutex);
}

int thread_lock(mythread_mutex_t* mutex){
    return acquire_spinlock((int*)mutex);
}

int thread_unlock(mythread_mutex_t* mutex){
    return release_spinlock((int*)mutex);
}

int thread_mutex_lock(mythread_mutex_t* mutex){
    return acquire_sleeplock((int*)mutex);
}

int thread_mutex_unlock(mythread_mutex_t* mutex){
    return release_sleeplock((int*)mutex);
}

int thread_cancel(mythread_t* thread){
    itimerval timer;
    getitimer(ITIMER_REAL,&timer);
    if(clearTimer()==-1){
        fprintf(stderr,"Failed to clear Timer\n");
        return -1;
    }

    while(__sync_lock_test_and_set(&listlock,1))
        ;
    thread_info* currThread=headThread;
    thread_info* foundThread=NULL;

    do{
        if(currThread->threadId==*thread){
            foundThread=currThread;
            break;
        }
        currThread=currThread->next;
    }while(currThread!=headThread);

    __sync_lock_release(&listlock);

    if(!foundThread){
        fprintf(stderr,"No such Thread ");
        return -1;
    }
    
    if(foundThread==headThread){
        foundThread->next->prev=foundThread->prev;
        foundThread->prev->next=foundThread->next;

        headThread=headThread->next;

        free(foundThread->stack);
        free(foundThread);
    }
    else{
        foundThread->next->prev=foundThread->prev;
        foundThread->prev->next=foundThread->next;
        
        free(foundThread->stack);
        free(foundThread);
    }

    setitimer(ITIMER_REAL,&timer,NULL);

    return 0;
}

int thread_kill(mythread_t* thread,int signal){
    if(!headThread){
        fprintf(stderr,"No such thread\n");
        return -1;
    }
    while(__sync_lock_test_and_set(&listlock,1))
        ;
    
    thread_info* currThread=headThread;
    thread_info* foundThread=NULL;

    do{
        if(currThread->threadId==*thread){
            foundThread=currThread;
            break;
        }
        currThread=currThread->next;
    }while(currThread!=headThread);

    __sync_lock_release(&listlock);

    if(!foundThread){
        fprintf(stderr,"No such Thread\n");
    }

    if(signal==SIGTERM || signal==SIGKILL){
        if(foundThread==headThread){
            foundThread->next->prev=foundThread->prev;
            foundThread->prev->next=foundThread->next;

            headThread=headThread->next;

            free(foundThread->stack);
            free(foundThread);
        }
        else{
            foundThread->next->prev=foundThread->prev;
            foundThread->prev->next=foundThread->next;
            
            free(foundThread->stack);
            free(foundThread);
        }
    }
    else if(signal==SIGTSTP){
        if(foundThread->state==RUNNABLE){
            foundThread->state=STOPPED;
        }
        else{
            if(foundThread->state==TERMINATED){
                fprintf(stderr,"Thread already terminated\n");
            }
            else{
                fprintf(stderr,"Thread already stopped\n");
            }
            return -1;
        }
    }
    else if(signal==SIGCONT){
        if(foundThread->state==TERMINATED){
            fprintf(stderr,"Thread already terminated\n");
            return -1;
        }
        foundThread->state=RUNNABLE;
    }
    else{
        fprintf(stderr,"Invalid signal\n");
        return -1;
    }
    return 0;
}


#endif
