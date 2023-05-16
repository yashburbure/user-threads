#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include <stdbool.h>
#include "thread.h"



int sum1=0;
int sum2=0;
int sum3=0;
int ct1,ct2;


int temp = 0;


mythread_mutex_t mutex1,mutex2,mutex3;

typedef struct yash{
    int a;
    int b;
    char ch;
}yash;

void f(void* arg){
    yash* y=(yash*)arg;
    printf("Inside thread %d %d %c\n",y->a,y->b,y->ch);
}
int main(){


    yash y;
    y.a=y.b=10;
    y.ch='y';
    mythread_t thread1;
    if(thread_create(&thread1,&f,(void*)(&y))==-1){
        return 1;
    }
    thread_join(&thread1,0);
    return 0;
}