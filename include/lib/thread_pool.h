#ifndef THREAD_POOL_H

#define THREAD_POOL_H

#include "job_queue.h"

typedef struct thread_pool_t {
    job_queue_t *__job_queue__;
    void (*do_work)(struct thread_pool_t *self, void *(*work)(void *args), void *args);
    void (*wait)(struct thread_pool_t *self);
} thread_pool_t;

thread_pool_t *init_thread_pool(int num_threads, job_queue_t *job_queue);
void destroy_thread_pool(thread_pool_t *self);

#endif