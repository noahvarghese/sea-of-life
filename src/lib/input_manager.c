#include "lib/input_manager.h"

static input_t **get_inputs(const input_manager_t *self) {
    return (input_t **)self->_inputs->items;
}

static void register_input(const input_manager_t *self, int glfw_key, void (*callback)(struct timeval *delta_time)) {
    vector_t *inputs = self->_inputs;

    input_t input = (input_t){
        glfw_key,
        callback,
    };

    inputs->push(inputs, &input);
}

input_manager_t *init_input_manager(input_t **inputs, int num_inputs) {
    input_manager_t *self;
    self = malloc(sizeof(input_manager_t));
    if (self == NULL) {
        error("Unable to allocate memory for input_manager");
    }

    self->get_inputs = get_inputs;
    self->register_input = register_input;

    self->_inputs = init_vector((void **)inputs, num_inputs, sizeof(input_t));

    return self;
}

void free_input_manager(input_manager_t *self) {
    free_vector(self->_inputs);
    free(self);
}