#define DEBUG

#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "life.h"
#include "lib/window_manager.h"
#include "lib/shader.h"

float vertices[] = {
   0.5f, 0.5f 
};

static struct settings_t {
    bool accelerate;
    int rows;
    int columns;
    float cell_width;
    vec3 dead_color;
    vec3 alive_color;
    vec3 background_color;
} settings = {
    true,               
    360,               
    860,              
    4.0,                
    {0.3, 0.3, 0.3},    
    {1.0, 1.0, 1.0},   
    {0.0, 0.0, 0.0},    
};

static struct uniforms_t {
    const char *accelerate;
    const char *cell;
    const char *grid_center;
    const char *cell_width;
    const char *alive_color;
    const char *dead_color;
    const char *model;
    const char *view;
    const char *projection;
} uniforms = {
    "accelerate",
    "cell",
    "gridCenter",
    "cellWidth",
    "aliveColor",
    "deadColor",
    "model",
    "view",
    "projection"
};

int width = 1;
int height = 1;
unsigned int VAO, VBO;

window_manager_t *window_manager;
shader_t *shader = NULL;
life_t *life;
vec2 grid_center = {};

static void load_settings() {
    if (shader == NULL)
        return;

    grid_center[0] = (float)(settings.rows / 2);
    grid_center[1] = (float)(settings.columns / 2);

    shader->use(shader);
    shader->setUniformV3F(shader, uniforms.alive_color, settings.alive_color[0], settings.alive_color[1], settings.alive_color[2]);
    shader->setUniformV3F(shader, uniforms.dead_color, settings.dead_color[0], settings.dead_color[1], settings.dead_color[2]);
    shader->setUniformFloat(shader, uniforms.cell_width, settings.cell_width);
    shader->setUniformV2F(shader, uniforms.grid_center, grid_center[0], grid_center[1]);
    shader->setUniformBool(shader, uniforms.accelerate, settings.accelerate);

    mat4 projection;
    glm_mat4_identity(projection);

    float num_cells_in_width = width / settings.cell_width;
    float num_cells_in_height = height / settings.cell_width;

    float left = num_cells_in_width / -2.0f;
    float right = num_cells_in_width / 2.0f;
    float bottom = num_cells_in_height / -2.0f;
    float top = num_cells_in_height / 2.0f;
    float near = 0.0f;
    float far = 100.0f;

    glm_ortho(left, right, bottom, top, near, far, projection);
    shader->setUniformM4F(shader, uniforms.projection, projection);
}

static void on_window_resize(GLFWwindow *_, int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    load_settings();
}

static void init_graphics(void) {
    window_manager = init_window_manager();
    window_manager->open_window(window_manager, "Sea of Life", 800, 800, NULL);
    window_manager->set_resize_callback(window_manager, on_window_resize);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_DEPTH_TEST);

    shader = init_shader("./assets/shaders/cell.vert", "./assets/shaders/cell.frag");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

static void destroy_graphics(void) {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    destroy_shader(shader);

    window_manager->close_window(window_manager);
    free_window_manager(window_manager);
}

void game_loop(void)
{
    life->live(life);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(
        settings.background_color[0], 
        settings.background_color[1], 
        settings.background_color[2], 
        1.0f
    );

    for (int i = 0; i < settings.rows; i++) {
        for (int j = 0; j < settings.columns; j++) {
            if (!settings.accelerate) {
                mat4 view;
                glm_mat4_identity(view);
                glm_translate(view, (vec3){j - grid_center[1], i - grid_center[0], -1.0f});
                shader->setUniformM4F(shader, uniforms.view, view);
            }

            shader->setUniformV3F(
                shader, 
                uniforms.cell, 
                (float)i, 
                (float)j, 
                (float)life->get_alive(life, i , j)
            );
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }
}

void init(void) {
    life = init_life(settings.rows, settings.columns);
    life->seed(life);
    init_graphics();
}

void destroy(void) {
    destroy_life(life);
    destroy_graphics();
}

int main(void) {
    init();
    window_manager->render(window_manager, game_loop, &(struct timeval){ 0, 16000 /* 60 fps == 16ms == 16000us */ });
    destroy();
}