
#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct spinlock{
    isLocked;
}spinlock;

spinlock* newLock();
void acquire(spinlock* spl,itimerval* timer);
void realease(spinlock* spl,itimerval* timer);





#endif