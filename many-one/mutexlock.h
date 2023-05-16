#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

typedef struct mythread_mutex_info{
    int mutexId;
    int isLocked;
    struct mythread_mutex_info* next;
}mythread_mutex_info;

int mutex_init_wrapper(int*);
int mutex_destroy_wrapper(int*);
mythread_mutex_info* find_mutex(int*);



#endif