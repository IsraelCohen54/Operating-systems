#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "osqueue.h"
#include <pthread.h>
typedef struct func_args
{
   void (*functionPtr)(void*); //pointer to function
   void *params;
}Func_Args;

typedef struct thread_pool
{
 int thread_num;
 OSQueue *tasks;
 pthread_cond_t cond_tasks;
 pthread_t* threads_ptr;
 int tasks_overall_num;
 int tasks_current_num;
 int tasks_done;
 int Alive;
 int mutex_off;
 int task_can_enter;
 pthread_mutex_t mutex_quoue;
}ThreadPool;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

//enter new mission into queue missions in threadpool
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif
