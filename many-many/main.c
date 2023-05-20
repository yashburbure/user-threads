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

#define LOOPVALUE 1000000
#define THREAD_NUMBER 6
int ct[THREAD_NUMBER];


void f(void* arg){
    int temp=*(int*)arg;
    for(int i=0;i<LOOPVALUE;i++){
        ct[temp]++;
        // printf("inside thread -----%d --- %d\n",temp,i);
    }
    printf("%d==\n",ct[temp]);
    thread_exit(0);
}

int temp[THREAD_NUMBER];
int main(){
    mythread_t thread[THREAD_NUMBER];
    for(int i=0;i<THREAD_NUMBER;i++){
        temp[i]=i;
        // printf("%d\n",temp[i]);
        if(thread_create(&thread[i],f,(void*)(&temp[i]))==-1){
            return 1;
        }
        int a=0;
        for(int j=0;j<1000000;j++){
            a++;
        }
        printf("%d\n",a);
        thread_kill(&thread[i],SIGKILL);
    }
    for(int i=0;i<THREAD_NUMBER;i++){
        printf("%d\n",ct[i]);
    }
    return 0;
}