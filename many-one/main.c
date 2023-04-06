#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include"thread.h"


void function1(void){
    for(int i=0;i<10;i++){
        printf("hello1\n");
    }
    thread_exit(0);
}


void function2(void){
    for(int i=0;i<10;i++){
        printf("hello2\n");
    }
}

int main(){
    mythread_t thread;

    if(thread_create(&thread,&function1,0)==1){
        return 1;
    }
    if(thread_create(&thread,&function2,0)==1){
        return 1;
    }
    printf("Exited\n");
    return 0;
}