#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include <stdbool.h>
#include "thread.h"



int sum1=0;
int sum2=0;
int sum3=0;
int ct1,ct2;


int temp = 0;


mythread_mutex_t mutex1,mutex2,mutex3;

int lst=10000000;

void function1(void *arg){
    for(int i=0;i<lst;i++){
        sum1++;
    }
    thread_exit("THREAD1 EXITED\n");
}


void function2(void *arg){
    for(int i=0;i<lst;i++){
        sum1++;
    }
    thread_exit("THREAD2 EXITED\n");
}


void function3(void *arg){
    for(int i=0;i<lst;i++){
        thread_lock(&mutex1);
        sum2++;
        thread_unlock(&mutex1);
    }
    thread_exit("THREAD3 EXITED\n");
}


void function4(void *arg){
    for(int i=0;i<lst;i++){
        thread_lock(&mutex1);
        sum2++;
        thread_unlock(&mutex1);
    }
    thread_exit("THREAD4 EXITED\n");
}

void function5(void *arg){
    for(int i=0;i<lst;i++){
        thread_mutex_lock(&mutex2);
        sum3++;
        thread_mutex_unlock(&mutex2);
    }
    thread_exit("THREAD5 EXITED\n");
}


void function6(void *arg){
    for(int i=0;i<lst;i++){
        thread_mutex_lock(&mutex2);
        sum3++;
        thread_mutex_unlock(&mutex2);
    }
    thread_exit("THREAD6 EXITED\n");
}






int main(){
    mythread_t thread1,thread2,thread3,thread4,thread5,thread6;

    thread_mutex_init(&mutex1);
    thread_mutex_init(&mutex2);

    thread_create(&thread1,&function1,0);
    thread_create(&thread2,&function2,0);
    thread_create(&thread3,&function3,0);
    thread_create(&thread4,&function4,0);
    thread_create(&thread5,&function5,0);
    thread_create(&thread6,&function6,0);



    
    
    
    void* returnValue;

    if(thread_join(&thread1,&returnValue)!=-1){
        printf("%s\n",(char*)returnValue);
        free(returnValue);
    }

    if(thread_join(&thread2,&returnValue)!=-1){
        printf("%s\n",(char*)returnValue);
        free(returnValue);
    }
    
    if(thread_join(&thread3,&returnValue)!=-1){
        printf("%s\n",(char*)returnValue);
        free(returnValue);
    }
   

    
    if(thread_join(&thread4,&returnValue)!=-1){
        printf("%s\n",(char*)returnValue);
        free(returnValue);
    }
    


    if(thread_join(&thread5,&returnValue)!=-1){
        printf("%s\n",(char*)returnValue);
        free(returnValue);
    }
    


    if(thread_join(&thread6,&returnValue)!=-1){
        printf("%s\n",(char*)returnValue);
        free(returnValue);
    }


    

    printf("SUM WITHOUT LOCKING %d\n",sum1);
    printf("SUM WITH SPIN LOCKING %d\n",sum2);
    printf("SUM WITH SLEEP LOCKING %d\n",sum3);
    return 0;
}