#include "utils/int_tuple_list.h"

static void for_each(int_tuple_list_t *self, void (*delegate)(int coordinate[2], int index, void *context), void *context)
{
    for (int i = 0; i < self->_length; i++) {
        delegate(self->_list[i], i, context);
    }
}

/**
 * Doubles the current _size
*/
static void expand(int_tuple_list_t *self) {
    assert(self->_capacity == self->_length);

    self->_capacity = self->_length * 2;

    int **tmp;
    tmp = (int **)calloc(self->_capacity, sizeof(int *));
    if (tmp == NULL) {
        error("Unable to allocate memory to expand int_tuple_list::_list");
    }


    for (int i = 0; i < self->_capacity; i++) {
        tmp[i] = (int *)calloc(2, sizeof(int));
        if (tmp[i] == NULL) {
            error("Unable to allocate memory to copy element in int_tuple_list::_list item");
        }
        if (i < self->_length) {
            tmp[i][0] = self->_list[i][0];
            tmp[i][1] = self->_list[i][1];
        }
    }

    free(self->_list);
    self->_list = tmp;
}

static void push(int_tuple_list_t *self, int coordinate[2])
{
    self->_length++;

    if (self->_length == self->_capacity)
    {
        expand(self);
    }

    int index = self->_length - 1;

    self->_list[index][0] = coordinate[0];
    self->_list[index][1] = coordinate[1];
}

static int *pop(int_tuple_list_t *self) {
    int *result = self->_list[self->_length - 1];
    self->_length--;
    return result;
}

static void clear(int_tuple_list_t *self) {
    self->_length = 0;
}

/**
 * If initialValues is null, will initialize to provided size.
 * Otherwise will copy [size] members from initialValues.
*/
int_tuple_list_t *init_int_tuple_list(int size, int **initialValues) {
    bool has_values = initialValues != NULL;

    int_tuple_list_t *self = (int_tuple_list_t *)calloc(1, sizeof(int_tuple_list_t));
    if (self == NULL) {
        error("Unable to allocate memory for int_tuple_list");
    }

    printf("Allocated self\n");

    self->_length = has_values ? size : 0;
    self->_capacity = size;

    self->_list = (int **)calloc(self->_capacity, sizeof(int *));
    if (self->_list == NULL) {
        error("Unable to allocate memory for int_tuple_list::_list");
    }

    printf("Allocated list\n");

    for (int i = 0; i < self->_capacity; i++) {
        self->_list[i] = (int *)calloc(2, sizeof(int));
        if (self->_list[i] == NULL) {
            error("Unable to allocate memory for int_tuple_list::_list item");
        }

        printf("Allocated %d\n", i);
        if (has_values) {
            self->_list[i][0] = initialValues[i][0];
            self->_list[i][1] = initialValues[i][1];
        }
    }

    self->for_each = for_each;
    self->push = push;
    self->pop = pop;
    self->clear = clear;
    return self;
}

void destroy_int_tuple_list(int_tuple_list_t *self) {
    free(self->_list);
    free(self);
}