#include"mutexlock.h"
#include"spinlock.h"
#include<stdio.h>

#ifndef SPIN_LOCK_C
#define SPIN_LOCK_C


int acquire_spinlock(int* mutex){
    mythread_mutex_info* found_mutex=find_mutex(mutex);

    if(!found_mutex){
        fprintf(stderr,"No such Mutex\n");
        return -1;
    }
    while(__sync_lock_test_and_set(&(found_mutex->is_locked),1))
        ;
    return 0;
}


int release_spinlock(int* mutex){
    mythread_mutex_info* found_mutex=find_mutex(mutex);

    if(!found_mutex){
        fprintf(stderr,"No such Mutex\n");
        return -1;
    }
    __sync_lock_release(&(found_mutex->is_locked));
    return 0;
}

#endif