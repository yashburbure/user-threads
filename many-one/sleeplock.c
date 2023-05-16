#include"sleeplock.h"
#include"mutexlock.h"
#include"timer.h"
#include"thread.h"
#include<stdio.h>


#ifndef SLEEPLOCK_C
#define SLEEPLOCK_C

extern thread_info* headThread;
extern int listlock;
extern ucontext_t schedulerContext;

int acquire_sleeplock(int* mutex){
    mythread_mutex_info* foundMutex=find_mutex(mutex);

    if(!foundMutex){
        fprintf(stderr,"No such Mutex\n");
        return -1;
    }
    while(__sync_lock_test_and_set(&(foundMutex->isLocked),1)){
        if(clearTimer()==-1){
            fprintf(stderr,"Failed to acquire lock\n");
            return -1;
        }
        thread_info* currThread=headThread;

        while(__sync_lock_test_and_set(&listlock,1))
            ;
        headThread=headThread->next;
        
        __sync_lock_release(&listlock);

        swapcontext(&currThread->context,&schedulerContext);
    }

    return 0;
}

int release_sleeplock(int* mutex){
    mythread_mutex_info* foundMutex=find_mutex(mutex);

    if(!foundMutex){
        fprintf(stderr,"No such Mutex\n");
        return -1;
    }

    __sync_lock_release(&foundMutex->isLocked);
    return 0;

}



#endif