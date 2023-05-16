#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include"thread.h"



int sum1=0;
int sum2=0;
int sum3=0;
int ct=0;



#define LOOPVALUE 1000000

void function(void* arg){
    printf("Inside ")
}

int main(){
    mythread_t thread1,thread2,thread3;
    if(thread_create())
    return 0;
}