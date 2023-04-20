#include<sys/time.h>
#ifndef TIMER_H
#define TIMER_H

typedef struct itimerval itimerval;


int setTimer(int second,int microsecond);
int clearTimer();


#endif