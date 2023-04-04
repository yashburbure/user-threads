#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include"thread.h"


void function(void){
    printf("hello\n");
}


int main(){
    mythread_t thread;

    
    if(thread_create(&thread,&function,0)==1){
        return 1;
    }
    
    printf("Exited\n");
    return 0;
}