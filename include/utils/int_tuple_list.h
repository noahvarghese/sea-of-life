#ifndef INT_TUPLE_LIST_H

#define INT_TUPLE_LIST_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "std_utils.h"

typedef struct int_tuple_list_t {
    /**
     * The length of tracked items 
    */
    int _length;
    int _capacity;
    int **_list;
    void (*for_each)(struct int_tuple_list_t *self, void (*delegate)(int coordinate[2], int index, void *context), void *context);
    void (*push)(struct int_tuple_list_t *self, int coordinate[2]);
    int *(*pop)(struct int_tuple_list_t *self);
    /**
     * Resets the length 
    */
    void (*clear)(struct int_tuple_list_t *self);
} int_tuple_list_t;

int_tuple_list_t *init_int_tuple_list(int size, int **initialValues);
void destroy_int_tuple_list(int_tuple_list_t *self);

#endif