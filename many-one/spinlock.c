#include"spinlock.h"
#include"timer.h"
#include<malloc.h>

#ifndef SPINLOCK_C
#define SPINLOCK_C



spinlock* newLock(){
    spinlock* lock=(spinlock*)malloc(sizeof(spinlock));
    if(!lock){
        perror("Error : ");
        return NULL;
    }
    lock->isLocked=0;
    return lock;
}
int acquire(spinlock* spl){
    if(resetTimer()==-1){
        perror("Error : ");
        return -1;
    }
    spl->isLocked=1;

}
int realease(spinlock* spl){
    spl->isLocked=0;
    return setTimer(0,10000);
}




#endif