#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include"thread.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>


void function1(void){
    int ct=0;
    for(int i=0;i<1000000;i++){
        ct++;
    }
    printf("%d\n",ct);
    thread_exit(0);
}


void function2(void){
    int ct=0;
    for(int i=0;i<1000000;i++){
        ct++;
    }
    printf("%d\n",ct);
    thread_exit(0);
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