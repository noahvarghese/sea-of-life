#ifndef INPUT_MANAGER_H

#define INPUT_MANAGER_H

#include <stdlib.h>
#include "lib/vector.h"
#include "utils/std_utils.h"

typedef struct input_t {
    int key;
    /**
     * Accepts time since last render
    */
    void (*callback)(struct timeval *delta_time);
} input_t;

typedef struct input_manager_t
{
    vector_t *_inputs;
    void (*register_input)(const struct input_manager_t *self, int key, void (*callback)(struct timeval *delta_time));
    input_t **(*get_inputs)(const struct input_manager_t *self);
} input_manager_t;

input_manager_t *init_input_manager(input_t **inputs, int num_inputs);
void free_input_manager(input_manager_t *self);

#endif