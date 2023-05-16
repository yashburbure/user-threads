#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include"thread.h"



int sum1=0;
int sum2=0;
int sum3=0;
int ct=0;

mythread_mutex_t mutex1,mutex2,mutex3;


#define LOOPVALUE 1000000

void f(void* arg){
    int threadNumber=*(int*)arg;
    for(int i=0;i<10;i++){
        printf("THREAD NUMBER : %d :: %d\n",i);
    }
}

// void function1(void* arg){
//     printf("in function1\n");
//     for(int i=0;i<LOOPVALUE;i++){
//         sum1++;
//     }
//     thread_exit("THREAD1 EXITED\n");
// }


// void function2(void* arg){
//     printf("in function2\n");
//     for(int i=0;i<LOOPVALUE;i++){
//         sum1++;
//     }
//     thread_exit("THREAD2 EXITED\n");
// }
// void function2extended(void* arg){
//     for(int i=0;i<LOOPVALUE;i++){
//         sum1++;
//     }
//     thread_exit("THREADextended EXITED\n");
// }

// void function3(void* arg){
//     printf("in function3\n");
//     for(int i=0;i<LOOPVALUE;i++){
//         thread_lock(&mutex1);
//         sum2++;
//         thread_unlock(&mutex1);
//     }
//     thread_exit("THREAD3 EXITED\n");
// }


// void function4(void* arg){
//     printf("in function4\n");
//     for(int i=0;i<LOOPVALUE;i++){
//         thread_lock(&mutex1);
//         sum2++;
//         thread_unlock(&mutex1);
//     }
//     thread_exit("THREAD4 EXITED\n");
// }

// void function5(void* arg){
//     for(int i=0;i<LOOPVALUE;i++){
//         thread_mutex_lock(&mutex2);
//         sum3++;
//         thread_mutex_unlock(&mutex2);
//     }
//     thread_exit("THREAD5 EXITED\n");
// }


// void function6(void* arg){
//     for(int i=0;i<LOOPVALUE;i++){
//         thread_mutex_lock(&mutex2);
//         sum3++;
//         thread_mutex_unlock(&mutex2);
//     }
//     thread_exit("THREAD6 EXITED\n");
// }

// void function7(void* arg){
//     int a=*(int*)arg;
//     printf("%d --------\n",a);
//     thread_exit(0);
// }


int main(){
    mythread_t thread1,thread2,thread3,thread4,thread5,thread6,thread7;
    
    if(thread_create(&thread1,&f,))


    // thread_mutex_init(&mutex1);

    // if(thread_create(&thread1,&function1,0)==-1){
    //     return 1;
    // }
    // if(thread_create(&thread2,&function2,0)==-1){
    //     return 1;
    // }

    // if(thread_create(&thread7,&function2extended,0)==-1){
    //     return 1;
    // }




    if(thread_create(&thread3,&function3,0)==1){
        return 1;
    }
    if(thread_create(&thread4,&function4,0)==1){
        return 1;
    }
    // if(thread_create(&thread5,&function5,0)==1){
    //     return 1;
    // }
    // if(thread_create(&thread6,&function6,0)==1){
    //     return 1;
    // }
    // if(thread_join(&thread1,NULL)==-1){
    //     return 1;
    // }
    // printf("Thread 1 joined\n");
    // if(thread_join(&thread2,NULL)==-1){
    //     return 1;
    // }
    // printf("Thread 2 joined\n");
    // if(thread_join(&thread3,NULL)==-1){
    //     return 1;
    // }
    // printf("Thread 3 joined\n");
    // if(thread_join(&thread4,NULL)==-1){
    //     return 1;
    // }
    // printf("Thread 4 joined\n");
    // if(thread_join(&thread5,NULL)==-1){
    //     return 1;
    // }
    // if(thread_join(&thread6,NULL)==-1){
    //     return 1;
    // }


    // thread_kill(&thread1,SIGTSTP);
    // // printf("%d\n",ct);
    // // printf("%d----%d\n",thread1,thread2);
   
    
    
    // void* returnValue;
    // if(thread_join(&thread1,&returnValue)==-1){
    //     return 1;
    // }
    // printf("%s",(char*)returnValue);

    // free(returnValue);
    // if(thread_join(&thread2,&returnValue)==-1){
    //     return 1;
    // }
    // printf("%s",(char*)returnValue);
    // free(returnValue);
    // // if(thread_join(&thread7,&returnValue)==-1){
    // //     return 1;
    // // }
    // // printf("%s",(char*)returnValue);
    // // free(returnValue);

    // if(thread_join(&thread3,&returnValue)==-1){
    //     return 1;
    // }
    // printf("%s",(char*)returnValue);
    // free(returnValue);
    // if(thread_join(&thread4,&returnValue)==-1){
    //     return 1;
    // }
    // printf("%s\n",(char*)returnValue);
    // free(returnValue);
    // if(thread_join(&thread5,&returnValue)==-1){
    //     return 1;
    // }
    // printf("%s\n",(char*)returnValue);
    // free(returnValue);
    // if(thread_join(&thread6,&returnValue)==-1){
    //     return 1;
    // }
    // printf("%s\n",(char*)returnValue);
    // free(returnValue);

    // printf("ct : %d\n",ct);

    // thread_kill(&thread1,SIGCONT);


    printf("SUM WITHOUT LOCKING %d\n",sum1);
    printf("SUM WITH SPIN LOCKING %d\n",sum2);
    // printf("SUM WITH SLEEP LOCKING %d\n",sum3);

    return 0;
}