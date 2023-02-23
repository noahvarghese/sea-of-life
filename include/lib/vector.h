#ifndef VECTOR_H

#define VECTOR_H

#include <stdlib.h>
#include "utils/std_utils.h"

typedef struct vector_t {
    int _capacity;
    size_t _item_size;
    int length;
    void **items;
    void (*push)(struct vector_t *self, void *item);
    void *(*pop)(struct vector_t *self);
} vector_t;

vector_t *init_vector(void **items, int num_items, size_t item_size);
void free_vector(vector_t *self);

#endif