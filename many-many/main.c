#include"thread.h"
#include<stdio.h>

void function(void* arg){
    printf("Hello\n");
}

int main(){
    mythread_t thread;
    if(thread_create(&thread,&function,NULL)==-1){
        printf("Error \n");
    }


    return 0;
}