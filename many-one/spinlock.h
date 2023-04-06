
#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct spinlock{
    int isLocked;
}spinlock;

void initspinLock(spinlock* spl);
int acquire(spinlock* spl);
int release(spinlock* spl);





#endif