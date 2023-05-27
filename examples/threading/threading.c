#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    struct thread_data* td = (struct thread_data *)thread_param;
    td->thread_complete_success = false;//default
    msleep(td->wait_to_obtain_ms);
    int err1 = pthread_mutex_lock(td->mutex);
    if (err1) {
        ERROR_LOG("failed to obtain mutex");
        return td;
    }
    msleep(td->wait_to_release_ms);
    int err2 = pthread_mutex_unlock(td->mutex);
    if (err2) {
        ERROR_LOG("failed to release mutex");
        return td;
    }
    td->thread_complete_success = true;
    
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    struct thread_data *td = calloc(1, sizeof(struct thread_data));
    td->wait_to_obtain_ms = wait_to_obtain_ms;
    td->wait_to_release_ms = wait_to_release_ms;
    td->mutex = mutex;
    
    int err = pthread_create(thread, NULL, threadfunc, (void*)td);
    if(err){
    	return false
    }else{
    	return true
    }   
}

