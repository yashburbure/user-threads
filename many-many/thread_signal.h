#ifndef THREAD_SIGNAL_H
#define THREAD_SIGNAL_H

typedef struct signal_thread{
    int thread_id;
    int signal;
    struct signal_thread* next;
}signal_thread;

typedef struct signal_thread* signal_list;

void insert_signal(int thread_id,int signal);
void handle_signals(int kid);

#endif