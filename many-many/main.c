#include"thread.h"
#include<stdio.h>

void function(void* arg){
    int a=*(int*)arg;
    for(int i=0;i<10;i++){
        printf("Hello %d\n",a);
    }
}

int main(){
    mythread_t thread1,thread2,thread3;
    int a[]={1,2,3};
    if(thread_create(&thread1,&function,(void*)(&a[0]))==-1){
        printf("Error \n");
    }
    if(thread_create(&thread2,&function,(void*)(&a[1]))==-1){
        printf("Error \n");
    }
    if(thread_create(&thread3,&function,(void*)(&a[2]))==-1){
        printf("Error \n");
    }
    printf("Ended\n");



    return 0;
}