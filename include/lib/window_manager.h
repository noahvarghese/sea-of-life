#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lib/input_manager.h"
#include "utils/std_utils.h"
#include "utils/string_utils.h"

#define GLFW_CONTEXT_VERSION_MAJOR_VALUE 4
#define GLFW_CONTEXT_VERSION_MINOR_VALUE 6
#define GLFW_OPENGL_PROFILE_VALUE GLFW_OPENGL_CORE_PROFILE

typedef enum {
    release = GLFW_RELEASE,
    press = GLFW_PRESS,
    // repeat = GLFW_REPEAT,
} action_e;

typedef struct input_config_t {
    input_t **inputs;
    int num_inputs;
} input_config_t;

typedef struct window_manager_t{
    input_manager_t *_input_manager;
    GLFWwindow *window;
    int width;
    int height;
    void (*close_window)(struct window_manager_t *self);
    void (*open_window)(struct window_manager_t *self, const char *title, int width, int height, GLFWmonitor *monitor);
    void (*set_resize_callback)(struct window_manager_t *self, void (*on_resize)(GLFWwindow *window, int width, int height));
    void (*set_mouse_scroll_callback)(struct window_manager_t *self, void (*on_mouse_scroll)(GLFWwindow *window, double xoffset, double yoffset));
    // void (*resize_window)(struct window_manager_t *self, int width, int height);
    // action_e (*poll_for_key_input)(struct window_manager_t *self, int key);
    // void (*process_inputs)(struct window_manager_t *self, struct timeval *delta_time);
    void (*get_window_dimensions)(struct window_manager_t *self, int *width, int *height);
    /**
     * If frame_duration is NULL then it will render as fast as possible
    */
    void (*render)(struct window_manager_t *self, void (*loop)(struct timeval *delta_time), const struct timeval *frame_duration);
    void (*render_once)(struct window_manager_t *self, void (*loop)(struct timeval *delta_time), struct timeval *delta_time);
    void (*render_frame_limit)(struct window_manager_t *self, void (*loop)(struct timeval *delta_time), struct timeval frame_duration, struct timeval *prev_render);
} window_manager_t;

window_manager_t *init_window_manager(input_config_t input_config);
void free_window_manager(window_manager_t *self);

#endif