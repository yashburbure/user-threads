#include<malloc.h>
#include"spinlock.h"
#include"timer.h"

#ifndef SPINLOCK_C
#define SPINLOCK_C


void initspinLock(spinlock* spl){
    spl->isLocked=0;
}

void acquire(spinlock* spl){
    spl->isLocked=1;
}
void release(spinlock* spl){
    spl->isLocked=0;
}




#endif