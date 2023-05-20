#include"mutexlock.h"
#include<malloc.h>

#ifndef MUTEXLOCK_C
#define MUTEXLOCK_C

int mutex_id=0;
int lock=0;
int listlockl=0;

mythread_mutex_info* head_mutex=NULL;
mythread_mutex_info* back_mutex=NULL;


mythread_mutex_info* newMutex(){
    mythread_mutex_info* nn=(mythread_mutex_info*)malloc(sizeof(mythread_mutex_info));
    if(!nn){
        perror("Error : ");
        return 0;
    }
    nn->is_locked=0;
    nn->next=NULL;

    while(__sync_lock_test_and_set(&lock,1))
        ;
    nn->mutex_id=mutex_id++;

    __sync_lock_release(&lock);

    return nn;
}

int mutex_init_wrapper(int* mutex){
    mythread_mutex_info* nn=newMutex();
    if(!nn){
        return -1;
    }

    *mutex=nn->mutex_id;

    while(__sync_lock_test_and_set(&listlockl,1))
        ;

    if(!head_mutex){
        head_mutex=nn;
        back_mutex=nn;
    }
    else{
        back_mutex->next=nn;
        back_mutex=nn;
    }

    __sync_lock_release(&listlockl);
    return 0;
}


int mutex_destroy_wrapper(int* mutex){
    while(__sync_lock_test_and_set(&listlockl,1))
        ;
    mythread_mutex_info* curr_mutex=head_mutex;
    mythread_mutex_info* prev_mutex;
    while(curr_mutex){
        if(curr_mutex->mutex_id==*mutex){
            break;
        }
        prev_mutex=curr_mutex;
        curr_mutex=curr_mutex->next;
    }
    
    if(!curr_mutex){
        fprintf(stderr,"No such Mutex\n");
        __sync_lock_release(&listlockl);
        return -1;
    }
    
    if(curr_mutex==head_mutex){
        head_mutex=head_mutex->next;
        if(!head_mutex){
            back_mutex=NULL;
        }
    }
    else if(curr_mutex==back_mutex){
        prev_mutex->next=NULL;
        back_mutex=prev_mutex;
    }
    else{
        prev_mutex->next=prev_mutex->next->next;
    }
    __sync_lock_release(&listlockl);
    free(curr_mutex);
    return 0;
}

mythread_mutex_info* find_mutex(int* mutex){
    while(__sync_lock_test_and_set(&listlockl,1))
        ;
    mythread_mutex_info* curr_mutex=head_mutex;
    
    while(curr_mutex){
        if(curr_mutex->mutex_id==*mutex){
            break;
        }
        curr_mutex=curr_mutex->next;
    }
    
    __sync_lock_release(&listlockl);

    return curr_mutex;
}


#endif