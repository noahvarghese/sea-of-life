#ifndef SHADER_H

#define SHADER_H

#include <stdbool.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "utils/std_utils.h"
#include "utils/string_utils.h"

typedef struct shader_t
{
    unsigned int ID;
    void (*use)(struct shader_t *self);
    void (*setUniformBool)(struct shader_t *self, const char *uniformVarName, bool val);
    void (*setUniformInt)(struct shader_t *self, const char *uniformVarName, int val);
    void (*setUniformFloat)(struct shader_t *self, const char *uniformVarName, float val);
    void (*setUniformV3F)(struct shader_t *self, const char *uniformVarName, float v1, float v2, float v3);
    void (*setUniformV4F)(struct shader_t *self, const char *uniformVarName, float v1, float v2, float v3, float v4);
    void (*setUniformM4F)(struct shader_t *self, const char *uniformVarName, mat4 mat);
} shader_t;

shader_t *init_shader(const char *vertex, const char *fragment);
void destroy_shader(shader_t *self);

#endif