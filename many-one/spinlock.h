
#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct spinlock{
    int isLocked;
}spinlock;

void initspinLock(spinlock* spl);
void acquire(spinlock* spl);
void release(spinlock* spl);





#endif