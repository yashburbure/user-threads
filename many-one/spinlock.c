#include<malloc.h>
#include"spinlock.h"
#include"timer.h"

#ifndef SPINLOCK_C
#define SPINLOCK_C


void initspinLock(spinlock* spl){
    spl->isLocked=0;
}

int acquire(spinlock* spl){
    if(clearTimer()==-1){
        perror("Error : ");
        return -1;
    }
    spl->isLocked=1;
    return 0;
}
int release(spinlock* spl){
    spl->isLocked=0;
    return setTimer(0,10000);
}




#endif