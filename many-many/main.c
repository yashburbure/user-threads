#include"thread.h"
#include<stdio.h>

int lock=0;

void function1(void* arg){
    for(int i=0;i<1000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,1);
        }
    }
    thread_exit(0);
}
void function2(void* arg){
    for(int i=0;i<1000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,2);
        }
    }
    thread_exit(0);
}
void function3(void* arg){
    for(int i=0;i<1000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,3);
        }
    }
    thread_exit(0);
}
void function4(void* arg){
    for(int i=0;i<1000000;i++){
        if(i%100000==0){
            printf("%d %d\n",i,4);
        }
    }
    thread_exit(0);
}

int main(){
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

    thread_join(&thread1,NULL);
    printf("Thread1 exited\n");
    thread_join(&thread2,NULL);
    printf("Thread2 exited\n");
    thread_join(&thread3,NULL);
    printf("Thread3 exited\n");
    thread_join(&thread4,NULL);
    printf("Thread4 exited\n");

    printf("Ended\n");



    return 0;
}