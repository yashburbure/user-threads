#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"thread.h"

#define N 500
#define THREAD_NUMBER 3

void f(void* arg){
    int matrix1[N][N]={0};
    int result[N][N];
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            for(int k=0;k<N;k++){
                result[i][j]=matrix1[i][j]*matrix1[j][k];
            }
        }
    }
}


int main(){
    clock_t t=clock();
    mythread_t threads[THREAD_NUMBER];
    for(int i=0;i<THREAD_NUMBER;i++){
        if(thread_create(&threads[i],f,NULL)==-1){
            fprintf(stderr,"failed\n");
            return 1;
        }
    }
    for(int i=0;i<THREAD_NUMBER;i++){
        if(thread_join(&threads[i],NULL)==-1){
            fprintf(stderr,"failed\n");
            return 1;
        }
    }
    t=clock()-t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Time taken by many-one model %f\n",time_taken);
    return 0;
}