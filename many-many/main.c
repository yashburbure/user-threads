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



#define LOOPVALUE 1000

void f(void* arg){
    int temp=*(int*)arg;
    for(int i=0;i<LOOPVALUE;i++){
        printf("inside thread ----- %d --- %d\n",temp,i);
    }
    thread_exit("Exited\n");
}

int temp[6];
int main(){
    mythread_t thread[6];
    for(int i=0;i<5;i++){
        temp[i]=i;
        printf("%d\n",temp[i]);
        if(thread_create(&thread[i],f,(void*)(&temp[i]))==-1){
            return 1;
        }
        // void* ptr;
        thread_join(&thread[i],NULL);
        // printf("%s\n",(char*)ptr);
    }
    return 0;
}