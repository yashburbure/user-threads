#define _GNU_SOURCE
#include"thread.h"
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>


int lock=0;

void function1(void* arg){
    printf("PROCESS ID 1 %d :: THREAD ID 1 %d\n",getpid(),gettid());
    printf("Function1\n");
    for(int i=0;i<10000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,1);
        }
    }
    thread_exit(0);
}
void function2(void* arg){
    printf("PROCESS ID 2 %d :: THREAD ID 2 %d\n",getpid(),gettid());
    printf("Function2\n");
    for(int i=0;i<1000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,2);
        }
    }
    thread_exit(0);
}
void function3(void* arg){
    printf("PROCESS ID 3 %d :: THREAD ID 3 %d\n",getpid(),gettid());
    printf("Function3\n");
    for(int i=0;i<1000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,3);
        }
    }
    thread_exit(0);
}
void function4(void* arg){
    printf("PROCESS ID 4 %d :: THREAD ID 4 %d\n",getpid(),gettid());
    printf("Function4\n");
    for(int i=0;i<1000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,4);
        }
    }
    thread_exit(0);
}

int main(){
    printf("PROCESS ID MAIN %d :: THREAD ID MAIN %d\n",getpid(),gettid());
    mythread_t thread1,thread2,thread3,thread4;
    if(thread_create(&thread1,&function1,NULL)==-1){
        printf("Error \n");
    }
    if(thread_create(&thread2,&function2,NULL)==-1){
        printf("Error \n");
    }
    if(thread_create(&thread3,&function3,NULL)==-1){
        printf("Error \n");
    }
    if(thread_create(&thread4,&function4,NULL)==-1){
        printf("Error \n");
    }
    int ct=0;
    while(ct<40000){
        ct++;
    }
    thread_cancel(&thread1);
    
    // thread_join(&thread2,NULL);
    // printf("Thread2 exited\n");
    // thread_join(&thread3,NULL);
    // printf("Thread3 exited\n");
    // thread_join(&thread4,NULL);
    // printf("Thread4 exited\n");

    printf("Ended\n");



    return 0;
}