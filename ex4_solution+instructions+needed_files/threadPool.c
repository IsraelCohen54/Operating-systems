// Israel Cohen 205812290
#include "threadPool.h"
#include <stdio.h>
#include <pthread.h>
#include "osqueue.h"
#include <stdlib.h>

void* thread_func(void* the_tp){
    int empty = -1;
    int to_make_sure = 0;
    while(((ThreadPool*)the_tp)->Alive)
    {   
        pthread_mutex_lock(&((ThreadPool*)the_tp)->mutex_quoue); //~~ lock
        
        if (((ThreadPool*)the_tp)->mutex_off==0)
        {
            pthread_cond_wait(&(((ThreadPool*)the_tp)->cond_tasks),&((ThreadPool*)the_tp)->mutex_quoue);
        }
        empty = osIsQueueEmpty(((ThreadPool*)the_tp)->tasks);
        pthread_mutex_unlock(&((ThreadPool*)the_tp)->mutex_quoue); //~~ lock
        if ((!empty) && (((ThreadPool*)the_tp)->tasks_current_num>0)) {
            ((ThreadPool*)the_tp)->tasks_current_num-=1;
            if (((ThreadPool*)the_tp)->tasks_current_num > -1) {
                
                Func_Args *one_task_data = (Func_Args*)(osDequeue(((ThreadPool*)the_tp)->tasks));
                
                if (one_task_data == NULL) {
                    continue;
                }
                    one_task_data->functionPtr(one_task_data->params);
                    free(one_task_data);
                    ((ThreadPool*)the_tp)->tasks_done+=1;
                    continue;
            }
            else{
                
                continue;
            }
        }
        else{
            to_make_sure+=1;
            
        }
        if (to_make_sure == 4) //mission entered before all others finished, as alive should still function...
        {
            if (((ThreadPool*)the_tp)->tasks_overall_num==((ThreadPool*)the_tp)->tasks_done)
            {
                ((ThreadPool*)the_tp)->Alive=0;
            }   
        }
    }
}

ThreadPool* tpCreate(int numOfThreads){
    ThreadPool *tp = malloc(sizeof(ThreadPool));
    tp->tasks = osCreateQueue();
    tp->Alive=1;
    tp->thread_num=numOfThreads;
    tp->tasks_current_num = 0;
    tp->tasks_overall_num = 0;
    tp->tasks_done = 0;
    tp->mutex_off = 0;
    tp->task_can_enter = 1;
    pthread_mutex_init(&tp->mutex_quoue,NULL)!=0;
    pthread_cond_init(&tp->cond_tasks,NULL);

    tp->threads_ptr = malloc(sizeof(pthread_t) * numOfThreads);
    //check if the allocation have been failed
    if (tp->threads_ptr == NULL) {
        free(tp);
        _exit(EXIT_FAILURE);
    }
    int i = 0;
    int err = -5;
    for (i = 0; i < numOfThreads; i++)
    {
        pthread_t pthr_t;
        err = pthread_create(&(tp->threads_ptr[i]) ,NULL ,thread_func ,tp);
        if (err != 0)
        {
            perror("can't create thread\n");
            continue;
        }
        else{
        }
    }
    return tp;
}

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks){
    //preven more tasks entering quoue:
    threadPool->task_can_enter = 0;
    if (shouldWaitForTasks == 0){ //kill threads even while havine more tasks in quoue
        threadPool->Alive = 0;
    }
    else { //wait untill finished:
        
        if(((threadPool->tasks_overall_num)==(threadPool->tasks_done))){
            threadPool->Alive=0;
        }
    }
    void* retval_should_be_zero_if_succeeded;
    int i=0;
        
    threadPool->mutex_off = 1;
    //pthread_join((pthread_t)(threadPool->threads_ptr[thread_counter]), retval_should_be_zero_if_succeeded);

    for (i = 0; i < threadPool->thread_num; i++) {
        if(((threadPool->tasks_overall_num)==(threadPool->tasks_done))){
            threadPool->Alive=0;
        }   
        pthread_cond_broadcast(&threadPool->cond_tasks);
        if (pthread_join(threadPool->threads_ptr[i], NULL)!=0){
            _exit(EXIT_FAILURE);
        }
    }
    while (!osIsQueueEmpty(threadPool->tasks))
    {
        Func_Args *one_task_data = (Func_Args*)(osDequeue(threadPool->tasks));

                if (one_task_data == NULL) {
                    break;
                }
                free(one_task_data);
    }
    
    osDestroyQueue(threadPool->tasks);
    pthread_cond_destroy(&threadPool->cond_tasks);
    pthread_mutex_destroy(&threadPool->mutex_quoue); //~~ destroy
    free(threadPool->threads_ptr);
    free(threadPool);
}

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param){
    
    //example of funcion call with params through ptrs:
    //void (*functionPtr)(void*); //create ptr to func that return void and get void*(=any type arg)
    //meybe change to void* return, as I dont know test functions... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //functionPtr = (void*)computeFunc; //ptr pointing to func (as computFunc arg as ptr already IMO)
    //(*functionPtr)(&param); //using func with params, working!!!

    Func_Args *task = malloc(sizeof(Func_Args));
    task->functionPtr = computeFunc;
    task->params = param;

    if (threadPool->task_can_enter==1)
    {   
        pthread_mutex_lock(&threadPool->mutex_quoue); //~~ lock

        osEnqueue(threadPool->tasks,task);
        threadPool->tasks_overall_num +=1;
        threadPool->tasks_current_num +=1;
        pthread_mutex_unlock(&threadPool->mutex_quoue); //~~ unlock
        //tell all thread to work again: (instead of signal, broadcast tell to all threads)
        pthread_cond_broadcast(&threadPool->cond_tasks);
        //printf("tast edded\n");
    }
    return 0;
}