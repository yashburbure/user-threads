#include"timer.h"
#include<sys/time.h>
#ifndef TIMER_C
#define TIMER_C



int setTimer(int second,int nanosecond){
    itimerval timer;
    
    timer.it_value.tv_sec=second;
    timer.it_value.tv_usec=nanosecond;

    timer.it_interval.tv_sec=0;
    timer.it_interval.tv_usec=0;


    return setitimer(ITIMER_REAL,&timer,0); 
}

int resetTimer(){
    itimerval timer;

    timer.it_value.tv_sec=0;
    timer.it_value.tv_usec=0;

    timer.it_interval.tv_sec=0;
    timer.it_interval.tv_usec=0;

    return setitimer(ITIMER_REAL,&timer,0);
}


#endif