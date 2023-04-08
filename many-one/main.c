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
        if(ct%100000==0){
            printf(" %d Function1\n",ct);
        }
        ct++;
    }
    
    printf("Function1 exiting\n");
    thread_exit(0);
}


void function2(void){
    int ct=0;
    for(int i=0;i<10000000;i++){
        if(ct%100000==0){
            printf("%d Function2\n",ct);
        }
        ct++;
    }
    printf("Function2 exiting\n");
    thread_exit(0);
}

void function3(void){
    // int ct=0;
    // for(int i=0;i<10000000;i++){
    //     if(ct%100000==0){
    //         printf("Function3\n");
    //     }
    //     ct++;
    // }
    printf("Thread3 exited\n");
    thread_exit(0);
}

// void function4(void){
//     int ct=0;
//     // 
//     printf("function4\n");
//     thread_exit(0);
// }

int main(){
    mythread_t thread1,thread2,thread3,thread4;
    if(thread_create(&thread1,&function1,0)==1){
        return 1;
    }
    if(thread_create(&thread2,&function2,0)==1){
        return 1;
    }
    if(thread_create(&thread3,&function3,0)==1){
        return 1;
    }
    void* returnValue;
    thread_join(&thread3,&returnValue);
    printf("THREAD EXITED\n");
    // if(thread_create(&thread4,&function4,0)==1){
    //     return 1;
    // }
    // void* returnValue;
    // if(thread_join(&thread1,&returnValue)==-1){
    //     return 1;
    // }
    // printf("%s\n",(char*)returnValue);

    return 0;
}
// #define _GNU_SOURCE
// #include <stdio.h>
// #include <ucontext.h>
// #include <unistd.h> 
// #include "thread.h"
// #include <pthread.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <syscall.h>

// ucontext_t main_ctx;
// int x = 20;

// void f1(void *arg) {
    
//     int p = *(int *)arg;
//     // printf("%d x= %d\n", p, x);
    
//     // while(1) {    
//     // }
//     printf("thread1 running\n");


    
//     printf("thread1 exiting\n");

//     thread_exit(0);
// }

// void f2(void *arg) {
//     int p = *(int *)arg;
//     // printf("%d x= %d\n", p, x);
    
//     printf("thread2 running\n");

//     printf("First of thread2\n");
//     // sleep(0.10); //100 milliseconds
//     // for(int i=0; i<1000000; i++) {
        
//     // }
//     // alarm(0);
//     printf("second of thread2\n");
    
//     printf("thread2 exiting\n");

//     thread_exit(0);
// }

// void f3(void *arg) {
//     int p = *(int *)arg;
//     // printf("%d x= %d\n", p, x);
    
//     printf("thread3 running\n");

//     for(int i=0; i<100000; i++) {
//         printf("3 %d\n", i);
//     }

//     printf("thread3 exiting\n");

//     thread_exit(0);
// }

// void f4(void *arg) {
//     int p = *(int *)arg;
//     // printf("%d x= %d\n", p, x);
    
//     printf("thread4 running\n");

//     for(int i=0; i<100000; i++) {
//         printf("4 %d\n", i);
//     }

//     printf("thread4 exiting\n");

//     thread_exit(0);
// }

// void f5(void *arg) {
//     int p = *(int *)arg;
//     // printf("%d x= %d\n", p, x);
    
//     printf("thread5 running\n");

//     // for(int i=0; i<100000; i++) {
        
//     // }

//     printf("thread5 exiting\n");

//     thread_exit(0);
// }



// int main() {

//     // signal(SIGALRM, sig_alarm_handler);

//     // getcontext(&main_ctx);
//     int tid1, tid2, tid3, tid4, tid5;
//     int arg1 = 10, arg2 = 20;

//     thread_create(&tid1, f1, &arg1);
//     thread_create(&tid2, f2, &arg2);
//     thread_create(&tid3, f3, &arg1);
//     thread_create(&tid4, f4, &arg2);
//     thread_create(&tid5, f5, &arg1);
//     // sleep(1); 
    
    
//     // thread_join(tid1);
//     // thread_join(tid2);
//     // thread_join(tid3);
//     // thread_join(tid4);
//     // thread_join(tid5);

//     return 0;
// }
