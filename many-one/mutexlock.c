#include"mutexlock.h"
#include<malloc.h>

#ifndef MUTEXLOCK_C
#define MUTEXLOCK_C

int mutexId=0;
int lock=0;
int listlockl=0;

mythread_mutex_info* headMutex=NULL;
mythread_mutex_info* backMutex=NULL;


mythread_mutex_info* newMutex(){
    mythread_mutex_info* nn=(mythread_mutex_info*)malloc(sizeof(mythread_mutex_info));
    if(!nn){
        perror("Error : ");
        return 0;
    }
    nn->isLocked=0;
    nn->next=NULL;

    while(__sync_lock_test_and_set(&lock,1))
        ;
    nn->mutexId=mutexId++;

    __sync_lock_release(&lock);

    return nn;
}

int mutex_init_wrapper(int* mutex){
    mythread_mutex_info* nn=newMutex();
    if(!nn){
        return -1;
    }

    *mutex=nn->mutexId;

    while(__sync_lock_test_and_set(&listlockl,1))
        ;

    if(!headMutex){
        headMutex=nn;
        backMutex=nn;
    }
    else{
        backMutex->next=nn;
        backMutex=nn;
    }

    __sync_lock_release(&listlockl);
    return 0;
}


int mutex_destroy_wrapper(int* mutex){
    while(__sync_lock_test_and_set(&listlockl,1))
        ;
    mythread_mutex_info* currMutex=headMutex;
    mythread_mutex_info* prevMutex;
    while(currMutex){
        if(currMutex->mutexId==*mutex){
            break;
        }
        prevMutex=currMutex;
        currMutex=currMutex->next;
    }
    
    if(!currMutex){
        fprintf(stderr,"No such Mutex\n");
        return -1;
    }
    
    if(currMutex==headMutex){
        headMutex=headMutex->next;
        if(!headMutex){
            backMutex=NULL;
        }
    }
    else if(currMutex==backMutex){
        prevMutex->next=NULL;
        backMutex=prevMutex;
    }
    else{
        prevMutex->next=prevMutex->next->next;
    }
    __sync_lock_release(&listlockl);
    free(currMutex);
    return 0;
}

mythread_mutex_info* find_mutex(int* mutex){
    while(__sync_lock_test_and_set(&listlockl,1))
        ;
    mythread_mutex_info* currMutex=headMutex;
    
    while(currMutex){
        if(currMutex->mutexId==*mutex){
            break;
        }
        currMutex=currMutex->next;
    }
    
    __sync_lock_release(&listlockl);

    return currMutex;
}


#endif