#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include"sleeplock.h"
#include"mutexlock.h"
#include"timer.h"
#include"thread.h"


#ifndef SLEEPLOCK_C
#define SLEEPLOCK_C

extern thread_info* head_thread[KTHREAD];
extern ucontext_t scheduler_context[KTHREAD];
extern int scheduler_thread_id[KTHREAD];



int acquire_sleeplock(int* mutex){
    mythread_mutex_info* found_mutex=find_mutex(mutex);

    if(!found_mutex){
        fprintf(stderr,"No such Mutex\n");
        return -1;
    }
    int kid=-1;
    for(int i=0;i<KTHREAD;i++){
        if(gettid()==scheduler_thread_id[i]){
            kid=i;
            break;
        }
    }
    if(kid==-1){
        return -1;
    }
    while(__sync_lock_test_and_set(&(found_mutex->is_locked),1)){
        if(clearTimer()==-1){
            perror("Failed to set timer\n");
            return -1;
        }
        swapcontext(&(head_thread[kid]->context),&(scheduler_context[kid]));
    }

    return 0;
}

int release_sleeplock(int* mutex){
    mythread_mutex_info* found_mutex=find_mutex(mutex);

    if(!found_mutex){
        fprintf(stderr,"No such Mutex\n");
        return -1;
    }

    __sync_lock_release(&found_mutex->is_locked);
    return 0;

}



#endif