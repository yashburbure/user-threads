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



int sum=0;
mythread_mutex_t mutex;

void function1(void){
    for(int i=0;i<1000000;i++){
        // thread_lock(&mutex);
        sum++;
        // thread_unlock(&mutex);
    }
    thread_exit("THREAD1 EXITED\n");
}


void function2(void){
    for(int i=0;i<1000000;i++){
        // thread_lock(&mutex);
        sum++;
        // thread_unlock(&mutex);
    }
    thread_exit("THREAD2 EXITED\n");
}

int main(){
    mythread_t thread1,thread2,thread3,thread4,thread5;

    thread_mutex_init(&mutex);
    printf("%d\n",(int)mutex);
    if(thread_create(&thread1,&function1,0)==1){
        return 1;
    }
    if(thread_create(&thread2,&function2,0)==1){
        return 1;
    }
    printf("%d----%d\n",thread1,thread2);
    void* returnValue;
    if(thread_join(&thread1,&returnValue)==-1){
        return 1;
    }
    printf("%s\n",(char*)returnValue);
    if(thread_join(&thread2,&returnValue)==-1){
        return 1;
    }

    if(thread_join(&thread2,&returnValue)==-1){
        return 1;
    }
    // printf("Thread joined\n");

    printf("%d\n",sum);
    return 0;
}