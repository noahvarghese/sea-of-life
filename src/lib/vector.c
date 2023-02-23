#include "lib/vector.h"

int default_capacity;
typedef struct input_t {
    int key;
    /**
     * Accepts time since last render
    */
    void (*callback)(struct timeval *delta_time);
} input_t;

static void increase_capacity(vector_t *self)
{
    self->_capacity *= 2;

    void **tmp;
    tmp = calloc(self->_capacity, self->_item_size);
    if (tmp == NULL)
        error("Unable to allocate memory to increase vector capacity");

    for (int i = 0; i < self->length; i++)
        tmp[i] = self->items[i];

    free(self->items);
    self->items = tmp;
}

static void push(vector_t *self, void *item)
{
    if (self->length == self->_capacity)
        increase_capacity(self);

    self->items[self->length++] = item;
}

static void *pop(vector_t *self) {
    if (self->length == 0)
        return NULL;

    void *item;
    item = self->items[self->length];

    self->items[self->length--] = NULL;

    return item;
}

vector_t *init_vector(void **items, int num_items, size_t item_size)
{
    vector_t *self;
    self = calloc(1, sizeof(vector_t));
    if (self == NULL) 
        error("Unable to allocate memory for vector");

    if (num_items < 0)
        error("number of items cannot be less than 0");

    self->_item_size = item_size;
    self->length = num_items;
    self->_capacity = num_items > default_capacity ? num_items * 2 : default_capacity;

    self->push = push;
    self->pop = pop;

    self->items = calloc(self->_capacity, self->_item_size);
    if (self->items == NULL) 
        error("Unable to allocate memory for vector items");

    if (items != NULL) 
        for (int i = 0; i < self->length; i++) 
            self->items[i] = items[i];

    return self;
}

void free_vector(vector_t *self) {
    for (int i = 0; i < self->length; i++)
    {
        free(self->items[i]);
    }

    free(self);
}