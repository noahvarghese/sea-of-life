#include "lib/thread_pool.h"
#include "lib/job_queue.h"
#include "utils/std_utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

static void do_work(thread_pool_t *self, void *(*work)(void *args), void *args) {
    self->__job_queue__->push(self->__job_queue__, work, args);
}

typedef struct args_t {
    job_queue_t *job_queue;
} args_t;

static void *poll(void *args) {
    args_t *thread_args = (args_t *)args;
    job_queue_t *job_queue = thread_args->job_queue;

    pthread_t tid;
    tid = pthread_self();
    int result;
    result = pthread_detach(tid);
    if (result != 0) {
        error("Unable to detach");
    }

    while (true)
    {
        work_item_t *work_item;
        work_item = job_queue->poll(job_queue);

        if (work_item != NULL)
        {
            work_item->work(work_item->args);
        }
    }

    return NULL;
}

static void wait(thread_pool_t *self) {
    job_queue_t *job_queue;
    job_queue = self->__job_queue__;

    while (job_queue->has_work(job_queue))
    {
        // do nothing...
    }

    return;
}

thread_pool_t *init_thread_pool(int num_threads, job_queue_t *job_queue) {
    thread_pool_t *self;

    self = calloc(1, sizeof(thread_pool_t));
    if (self == NULL) {
        error("Unable to allocate thread_pool");
    }

    self->__job_queue__ = job_queue;
    self->do_work = do_work;
    self->wait = wait;

    args_t *args;
    args = calloc(1, sizeof(args_t));
    if (args == NULL) {
        error("Unable to allocate args for thread");
    }

    args->job_queue = job_queue;

    for (int i = 0; i < num_threads; i++) {
        pthread_t thread_id;
        int result;
        result = pthread_create(&thread_id, NULL, poll, args);
        if (result != 0)
        {
            error("Unable to start thread");
        }
    }
    
    return self;
}

void destroy_thread_pool(thread_pool_t *self) {
    free(self);
}
