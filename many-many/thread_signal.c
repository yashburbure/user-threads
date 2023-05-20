#include"thread_signal.h"
#include"thread.h"
#include<signal.h>
#include<malloc.h>

#ifndef THREAD_SIGNAL_C
#define THREAD_SIGNAL_C

signal_list sig_list[KTHREAD];
signal_list sig_list_back[KTHREAD];

extern int listlock[KTHREAD];
extern thread_info* head_thread[KTHREAD];


extern void free_thread(thread_info* found_thread,int kid);

void init_signal_list(){
    for(int i=0;i<KTHREAD;i++){
        sig_list[i]=NULL;
        sig_list_back[i]=NULL;
    }
}

signal_thread* new_signal(int thread_id,int signal){
    static int initdone=0;
    if(!initdone){
        init_signal_list();
        initdone=1;
    }
    signal_thread* new_signal_node=(signal_thread*)malloc(sizeof(signal_thread));
    if(!new_signal_node){
        perror("Error : ");
        return NULL;
    }

    new_signal_node->thread_id=thread_id;
    new_signal_node->signal=signal;
    new_signal_node->next=NULL;

    return new_signal_node;
}

void insert_signal(int thread_id,int signal){
    signal_thread* new_signal_node=new_signal(thread_id,signal);
    if(!new_signal_node){
        return;
    }
    int kid=thread_id%KTHREAD;

    if(!sig_list[kid]){
        sig_list[kid]=sig_list_back[kid]=new_signal_node;
    }
    else{
        sig_list_back[kid]->next=new_signal_node;
        sig_list_back[kid]=new_signal_node;
    }
}
void handle_signals(int kid){
    while(sig_list[kid]){
        if(head_thread[kid]){
            int signal=sig_list[kid]->signal;
            while(__sync_lock_test_and_set(&listlock[kid],1))
                ;

            thread_info* curr_thread=head_thread[kid];
            thread_info* found_thread=NULL;
            do{
                if(curr_thread->thread_id==sig_list[kid]->thread_id){
                    found_thread=curr_thread;
                    break;
                }

                curr_thread=curr_thread->next;
            }while(curr_thread!=head_thread[kid]);
            
            if(found_thread){
                if(signal==SIGTERM || signal==SIGKILL){
                    free_thread(found_thread,kid);
                }
                else if(signal==SIGTSTP){
                    found_thread->state=TERMINATED;
                }
                else if(signal==SIGCONT){
                    if(found_thread->state==STOPPED){
                        found_thread->state=RUNNABLE;
                    }
                }
            }

            __sync_lock_release(&listlock[kid]);   
        }
        signal_thread* free_signal=sig_list[kid];
        sig_list[kid]=sig_list[kid]->next;
        free(free_signal);
    }
    sig_list_back[kid]=NULL;
}


#endif