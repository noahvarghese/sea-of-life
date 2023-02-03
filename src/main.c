#define DEBUG

#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "life.h"
#include "lib/window_manager.h"
#include "lib/shader.h"

// float vertices[] = {
//     0.95f, 0.95f, 0.0f,
//     0.95f, 0.05f, 0.0f,
//     0.05f, 0.05f, 0.0f,
//     0.05f, 0.95f, 0.0f};
float vertices[] = {
    1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f};


unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3};

static struct settings_t {
    int rows;
    int columns;
    vec3 dead_color;
    vec3 alive_color;
    vec3 background_color;
} settings = {
    860,              // rows
    360,              // columns
    {0.3, 0.3, 0.3}, // dead_color
    {1.0, 1.0, 1.0}, // alive_color
    {0.0, 0.0, 0.0}, // background_color
};

static struct uniforms_t {
    const char *row;
    const char *column;
    const char *is_alive;
    const char *rows;
    const char *columns;
    const char *cell_width;
    const char *alive_color;
    const char *dead_color;
    const char *model;
    const char *view;
    const char *projection;
} uniforms = {
    "row",
    "column",
    "is_alive",
    "rows",
    "columns",
    "cellWidth",
    "aliveColor",
    "deadColor",
    "model",
    "view",
    "projection"
};

unsigned int VAO;
unsigned int VBO;
unsigned int EBO;

window_manager_t *window_manager;
shader_t *shader;
life_t *life;

static void init_graphics(void) {
    window_manager = init_window_manager();
    window_manager->open_window(window_manager, "Sea of Life", 800, 800);
    shader = init_shader("./assets/shaders/cell.vert", "./assets/shaders/cell.frag");

    unsigned int VAO, VBO, EBO;

    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    shader->use(shader);
    shader->setUniformV3F(shader, uniforms.alive_color, settings.alive_color[0], settings.alive_color[1], settings.alive_color[2]);
    shader->setUniformV3F(shader, uniforms.dead_color, settings.dead_color[0], settings.dead_color[1], settings.dead_color[2]);

    mat4 model;
    glm_mat4_identity(model);
    shader->setUniformM4F(shader, uniforms.model, model);

    mat4 projection;

    float left = 0.0f;
    float right = settings.rows * 1.0f;
    float bottom = 0.0f;
    float top = settings.columns * 1.0f;
    float near = 0.1f;
    float far = 100.0f;

    glm_ortho(left, right, bottom, top, near, far, projection);
    shader->setUniformM4F(shader, uniforms.projection, projection);
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

    glDisable(GL_DEPTH_TEST);

    // Must clear before drawing (not after)
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(
        settings.background_color[0], 
        settings.background_color[1], 
        settings.background_color[2], 
        1.0f
    );

    for (int i = 0; i < settings.rows; i++) {
        for (int j = 0; j < settings.columns; j++) {
            mat4 view;
            glm_mat4_identity(view);
            glm_translate(view, (vec3){i * 1.0f, j * 1.0f, -1.0f});
            shader->setUniformM4F(shader, uniforms.view, view);

            shader->setUniformInt(shader, uniforms.is_alive, life->get_alive(life, i, j));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    window_manager->render(window_manager, game_loop, &(struct timeval){ 0, 16 });
    while(true) {}
    destroy();
}
