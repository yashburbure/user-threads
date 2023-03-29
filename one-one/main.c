#include"thread.h"
#include<stdio.h>
#include<unistd.h>

int function1(void* arg){
    for(int i=0;i<5;i++){
        printf("Hello1\n");
        sleep(1);
    }
    thread_exit(0);
    return 0;
}

int function2(void* arg){
    for(int i=0;i<1;i++){
        printf("Hello2\n");
        sleep(1);
    }
    thread_exit(0);
    return 0;
}

int main(){
    mythread_t thread1,thread2;
    thread_create(&thread1,&function1,0);
    thread_create(&thread2,&function2,0);
    void* returnValue;
    thread_join(&thread2,&returnValue);

    // printf("Main Thread Exited\n");
    return 0;
}