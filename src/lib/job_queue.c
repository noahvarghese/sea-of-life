#include <assert.h>
#include <stdlib.h>
#include "lib/job_queue.h"
#include "utils/std_utils.h"

static void destroy_work_item(work_item_t *self) {
    free(self);
}

static work_item_t *init_work_item(void *(*work)(void *args), void *args) {
    work_item_t *self;

    self = calloc(1, sizeof(work_item_t));
    if (self != NULL) {
        error("Unable to allocate memory for work_item");
    }

    self->work = work;
    self->args = args;
    self->next = NULL;

    return self;
}

static work_item_t *poll(job_queue_t *self) {
    if (self->__queue__ == NULL) {
        return NULL;
    }

    work_item_t *current = self->__queue__;
    self->__queue__ = current->next;
    return current;
}

static void push(job_queue_t *self, void *(*work)(void *args), void *args) {
    work_item_t *work_item = init_work_item(work, args);

    if (self->__queue__ == NULL)
    {
        self->__queue__ = work_item;
        return;
    }

    work_item_t *previous = NULL;
    work_item_t *current = self->__queue__;
    work_item_t *next;

    // Inefficient but easy to reason about and quickly implement
    while (current != NULL) {
        next = current->next;
        current = next;
    }

    assert(previous != NULL);
    previous->next = work_item;
    return;
}

static bool has_work(job_queue_t *self) {
    if (self->__queue__ == NULL) {
        return false;
    }

    return true;
}

job_queue_t *init_job_queue() {
    job_queue_t *self;

    self = calloc(1, sizeof(job_queue_t));
    if (self == NULL) {
        error("Failed to allocate job_queue");
    }

    self->__queue__ = NULL;
    self->poll = poll;
    self->push = push;
    self->has_work = has_work;

    return self;
}

void destroy_job_queue(job_queue_t *self) {
    work_item_t *current = self->__queue__;
    work_item_t *next;

    while (current != NULL)
    {
        next = current->next;
        destroy_work_item(current);
        current = next;
    }

    free(self);
}
