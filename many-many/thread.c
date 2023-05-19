#define _GNU_SOURCE
#include<unistd.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<malloc.h>
#include<sched.h>
#include<signal.h>
#include<string.h>
#include<linux/futex.h>
#include<stdint.h>
#include<sys/time.h>
#include<sys/wait.h>
#include"thread.h"
#include"timer.h"

#ifndef THREAD_C
#define THREAD_C

int initdone;
int main_thread_id;
int thread_id;
int thread_id_lock;
thread_info* head_thread[KTHREAD];
int listlock[KTHREAD];

ucontext_t scheduler_context[KTHREAD];

int schduler_thread_id[KTHREAD];
int thread_number[KTHREAD];

int kthread_id(int threadid){
    return (threadid)%KTHREAD;
}

void free_thread_ds(int kthread){
    if(!head_thread[kthread]){
        return;
    }
    thread_info* curr_thread=head_thread[kthread];
    thread_info* next_thread;
    do{
        next_thread=curr_thread->next;
        free(curr_thread->stack);
        free(curr_thread->return_value);
        free(curr_thread);
        curr_thread=next_thread;
    }while(curr_thread!=head_thread[kthread]);

}


thread_info* new_thread(void* arg){
    thread_info* nn=(thread_info*)malloc(sizeof(thread_info));
    if(!nn){
        perror("Error : ");
        return NULL;
    }

    if(getcontext(&nn->context)==-1){
        perror("Error : ");
        free(nn);
        return NULL;
    }

    nn->stack=malloc(STACK_SIZE);
    
    if(nn->stack==NULL){
        perror("Error : ");
        free(nn);
        return NULL;
    }
    while(__sync_lock_test_and_set(&thread_id_lock,1))
        ;
    nn->thread_id=thread_id++;

    __sync_lock_release(&thread_id_lock);

    nn->context.uc_stack.ss_size=STACK_SIZE;
    nn->context.uc_stack.ss_sp=nn->stack;
    nn->context.uc_link=&scheduler_context[kthread_id(nn->thread_id)];

    nn->state=RUNNABLE;
    nn->next=nn->prev=nn->return_value=NULL;

    return nn;
}


void signalHandler(int signal){
    printf("TIMER CALLED\n");
    for(int i=0;i<KTHREAD;i++){
        if(schduler_thread_id[i]==(int)gettid()){
            swapcontext(&(head_thread[i]->context),&(scheduler_context[i]));
        }
    }
}


int scheduler(void* arg){
    int kthread_number=*(int*)arg;
    signal(SIGALRM,signalHandler);

    if(getcontext(&scheduler_context[kthread_number])==-1){
        perror("Error : ");
        return -1;
    }
    scheduler_context[kthread_number].uc_link=NULL;
    // printf("Scheduler started %d\n",kthread_number);
    int main_thread_killed=(kill(main_thread_id,0));
    while(!main_thread_killed || head_thread[kthread_number]){
        thread_info* next_runnable_thread=NULL;

        while(__sync_lock_test_and_set(&listlock[kthread_number],1))
            ;
        if(head_thread[kthread_number]){
            thread_info* curr_thread=head_thread[kthread_number];

            do{
                if(curr_thread->state==RUNNABLE){
                    next_runnable_thread=curr_thread;
                    break;
                }
                curr_thread=curr_thread->next;
            }while(curr_thread!=head_thread[kthread_number]);
        }

        __sync_lock_release(&listlock[kthread_number]);

        if(!next_runnable_thread && main_thread_killed){
            break;
        }
        else if(next_runnable_thread){
            // printf("KTHREAD %d FOUND RUNNABLE THREAD\n",kthread_number);
            setTimer(0,TIMER_TIME);
            head_thread[kthread_number]=next_runnable_thread;
            next_runnable_thread->state=RUNNING;
            swapcontext(&scheduler_context[kthread_number],&(next_runnable_thread->context));
            
            itimerval timer;
            getitimer(ITIMER_REAL,&timer);
            if(next_runnable_thread->state!=OPERATION && timer.it_value.tv_usec>0){
                next_runnable_thread->state=TERMINATED;
            }
        }
        
        if(next_runnable_thread){
            while(__sync_lock_test_and_set(&listlock[kthread_number],1))
                ;
            head_thread[kthread_number]=next_runnable_thread->next;
            if(next_runnable_thread->state==RUNNING){
                next_runnable_thread->state=RUNNABLE;
            }

            __sync_lock_release(&listlock[kthread_number]);
        }
        main_thread_killed=(kill(main_thread_id,0));
    }
    // printf("Scheduler ended %d\n",kthread_number);
    free_thread_ds(kthread_number);
    return 0;
}

int initThreadDS(){
    main_thread_id=(int)gettid();

    for(int i=0;i<KTHREAD;i++){
        head_thread[i]=NULL;

        thread_number[i]=i;
        void* stack=malloc(STACK_SIZE);
        if(!stack){
            perror("Error : ");
            return -1;
        }
        schduler_thread_id[i]=clone(&scheduler,(void*)(stack+STACK_SIZE),CLONE_VM|CLONE_FILES,(void*)(&thread_number[i]));
        if(schduler_thread_id[i]==-1){
            perror("Error : ");
            return -1;
        }
    }

    return 0;
}

int thread_create(mythread_t* thread,void(*function)(void*),void* arg){
    if(!initdone){
        if(initThreadDS()==-1){
            return -1;
        }
        initdone=1;
    }

    thread_info* nn=new_thread(arg);
    

    if(!nn){
        return -1;
    }

    makecontext(&nn->context,(void(*)())function,1,arg);
    *thread=nn->thread_id;
    int kid=kthread_id(nn->thread_id);
    while(__sync_lock_test_and_set(&listlock[kid],1))
        ;
    
    if(!head_thread[kid]){
        nn->prev=nn->next=nn;
        head_thread[kid]=nn;
    }
    else{
        nn->prev=head_thread[kid]->prev;
        nn->next=head_thread[kid];
        head_thread[kid]->prev->next=nn;
        head_thread[kid]->prev=nn;
    }

    __sync_lock_release(&listlock[kid]);
    printf("%d THREAD CREATION DONE\n",*thread);
    return 0;
}

int thread_join(mythread_t* thread,void** return_value){
    
    int kid=kthread_id(*thread);

    while(__sync_lock_test_and_set(&listlock[kid],1))
        ;

    thread_info* curr_thread=head_thread[kid];
    thread_info* found_thread=NULL;

    do{
        if(curr_thread->thread_id==*thread){
            found_thread=curr_thread;
            break;
        }
        curr_thread=curr_thread->next;
    }while(curr_thread!=head_thread[kid]);
    
    __sync_lock_release(&listlock[kid]);

    if(!found_thread){
        fprintf(stderr,"Thread not found\n");
        return -1;
    }
    int ct=0;
    while(found_thread->state!=TERMINATED)
        ;

    if(return_value){
        *return_value=found_thread->return_value;
    }

    while(__sync_lock_test_and_set(&listlock[kid],1))
        ;
    if(found_thread->next==found_thread){
        head_thread[kid]=NULL;
    }
    else if(found_thread==head_thread[kid]){
        found_thread->next->prev=found_thread->prev;
        found_thread->prev->next=found_thread->next;

        head_thread[kid]=head_thread[kid]->next;

        free(found_thread->stack);
        free(found_thread);
    }
    else{
        found_thread->next->prev=found_thread->prev;
        found_thread->prev->next=found_thread->next;

        free(found_thread->stack);
        free(found_thread);
    }

    __sync_lock_release(&listlock[kid]);
    return 0;
}

void thread_exit(void* return_value){
    printf("THREAD EXIT\n");
    if(clearTimer()==-1){
        fprintf(stderr,"Failed to clear Timer\n");
    }
    for(int i=0;i<KTHREAD;i++){
        if(schduler_thread_id[i]==gettid()){
            head_thread[i]->state=TERMINATED;
            head_thread[i]->return_value=return_value;
            swapcontext(&(head_thread[i]->context),&scheduler_context[i]);
        }
    }
}

int thread_cancel(mythread_t* thread){
}
int thread_kill(mythread_t* thread,int signal){

}

#endif