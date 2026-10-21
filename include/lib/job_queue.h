#ifndef JOB_QUEUE_H

#define JOB_QUEUE_H

#include <stdbool.h>

typedef struct work_item_t {
    void *(*work)(void *args);
    void *args;
    struct work_item_t *next;
} work_item_t;

typedef struct job_queue_t {
    work_item_t *__queue__;
    bool (*has_work)(struct job_queue_t *self);
    work_item_t *(*poll)(struct job_queue_t *self);
    void (*push)(struct job_queue_t *self, void *(*work)(void *args), void *args);
} job_queue_t;

job_queue_t *init_job_queue();
void destroy_job_queue(job_queue_t *self);

#endif