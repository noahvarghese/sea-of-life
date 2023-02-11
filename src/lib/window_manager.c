#include "lib/window_manager.h"

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void resize_window(window_manager_t *self, int width, int height) {
    self->width = width;
    self->height = height;
    framebuffer_size_callback(self->window, width, height);
    glfwSetFramebufferSizeCallback(self->window, &framebuffer_size_callback);
}

static void set_resize_callback(window_manager_t *self, void (*on_resize)(GLFWwindow *window, int width, int height)) {
    on_resize(NULL, self->width, self->height);
    glfwSetFramebufferSizeCallback(self->window, on_resize);
}

static action_e poll_for_key_input(window_manager_t *self, int key) {
    action_e action;
    action = glfwGetKey(self->window, key);
    return action;
}

static void close_window(window_manager_t *self) {
    glfwSetWindowShouldClose(self->window, true);
}

static void open_window(window_manager_t *self,  const char *title, int width, int height, GLFWmonitor *monitor) {
    if (self->window != NULL)
    {
        error("Must close current window before opening a new one");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_CONTEXT_VERSION_MAJOR_VALUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_CONTEXT_VERSION_MINOR_VALUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_PROFILE_VALUE);

    self->window = glfwCreateWindow(width, height, title, monitor, NULL);
    if (self->window == NULL)
    {
        error("Failed to create GLFW window\nPlease reboot and try again.");
    }

    glfwMakeContextCurrent(self->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        error("Failed to initialize GLAD");
    }
    
    resize_window(self, width, height);
}

static void get_window_dimensions(window_manager_t *self, int *width, int *height) {
    glfwGetWindowSize(self->window, width, height);
}

static void render_once(window_manager_t *self, void (*loop)(void)) {
    loop();
    glfwSwapBuffers(self->window);
    glfwPollEvents();
}

static void render_frame_limit(window_manager_t *self, void (*loop)(void), struct timeval interval, struct timeval *prev_render) {
    struct timeval curr_render;
    long int sec_diff;
    long int usec_diff;

    gettimeofday(&curr_render, NULL);

    sec_diff = curr_render.tv_sec - prev_render->tv_sec;
    usec_diff = curr_render.tv_usec - prev_render->tv_usec;


    bool should_render = false;

    if (sec_diff >= interval.tv_sec)
        should_render = true;
    else if (sec_diff == interval.tv_sec && usec_diff > interval.tv_usec)
        should_render = true;

    if (should_render)
    {
        printf("%ld, %ld\n\n", sec_diff, usec_diff);
        self->render_once(self, loop);

        prev_render->tv_sec = curr_render.tv_sec;
        prev_render->tv_usec = curr_render.tv_usec;
    }
}

static void render(window_manager_t *self, void (*loop)(void), const struct timeval *interval) {
    struct timeval render_time = { 0, 0 };

    while(!glfwWindowShouldClose(self->window)) {
        if (interval != NULL)
        {
            self->render_frame_limit(self, loop, *interval, &render_time);
        }
        else
        {
            self->render_once(self, loop);
        }
    }
}
window_manager_t *init_window_manager()
{
    if (!glfwInit())
    {
        error("Unable to init glfw");
    }

    window_manager_t *self;

    self = (window_manager_t *)malloc(sizeof(window_manager_t));
    if (self == NULL)
    {
        error("Unable to allocate memory for window");
    }

    self->poll_for_key_input = poll_for_key_input;
    // self->resize_window = resize_window;
    self->set_resize_callback = set_resize_callback;
    self->close_window = close_window;
    self->open_window = open_window;
    self->get_window_dimensions = get_window_dimensions;
    self->render = render;
    self->render_once = render_once;
    self->render_frame_limit = render_frame_limit;
    self->window = NULL;

    return self;
}

void free_window_manager(window_manager_t *self) {
    glfwDestroyWindow(self->window);
    glfwTerminate();
    free(self);
}