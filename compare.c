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

#define N 500
#define THREAD_NUMBER 3

void f(){
    int result[N][N];
    int matrix1[N][N]={0};
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
    f();
    f();
    f();

    t=clock()-t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Time taken by normal program %f\n",time_taken);
    return 0;
}
