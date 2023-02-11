#include "lib/shader.h"

static char *read_file(const char *fileName)
{
    FILE *file = fopen(fileName, "r");

    if (file == NULL)
    {
        error(str_concat("Unable to open file ", fileName));
    }

    fseek(file, 0L, SEEK_END);

    long lSize = ftell(file);
    rewind(file);

    // Leave room for null terminated string
    char *buffer = (char *)malloc(lSize + 1);

    if (buffer == NULL)
    {
        fclose(file);
        error("Unable to allocate memory to read file");
    }

    if (!fread(buffer, lSize, 1, file))
    {
        fclose(file);
        error(str_concat("Unable to read file ", fileName));
    }

    buffer[lSize] = '\0';

    fclose(file);
    return buffer;
}

static unsigned int compile_glsl_shader(const char *fileName, GLenum shaderType)
{
    const char *source = read_file(fileName);

    int success;
    char infoLog[512];
    unsigned int shader;

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // check for errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        const char *message = str_concat(str_concat(str_concat("Shader compilation failed for file: ", fileName), "\n"), infoLog);
        error(message);
    }

    return shader;
}

static int getUniformLocation(int ID, const char *variableName)
{
    int uniformVarLocation = glGetUniformLocation(ID, variableName);

    if (uniformVarLocation == -1)
    {
        const char *message = "Failed to get uniform location: ";
        error(str_concat(message, variableName));
    }

    return uniformVarLocation;
}

static void use(shader_t *self)
{
    glUseProgram(self->ID);
}

static void setUniformBool(shader_t *self, const char *uniformVarName, bool val)
{
    int location = getUniformLocation(self->ID, uniformVarName);
    glUniform1i(location, (int)val);
}

static void setUniformInt(shader_t *self, const char *uniformVarName, int val)
{
    int location = getUniformLocation(self->ID, uniformVarName);
    glUniform1i(location, val);
}

static void setUniformFloat(shader_t *self, const char *uniformVarName, float val)
{
    int location = getUniformLocation(self->ID, uniformVarName);
    glUniform1f(location, val);
}

static void setUniformV2F(shader_t *self, const char *uniformVarName, float v1, float v2) {
    int location = getUniformLocation(self->ID, uniformVarName);
    glUniform2f(location, v1, v2);
}

static void setUniformV3F(shader_t *self, const char *uniformVarName, float v1, float v2, float v3)
{
    int location = getUniformLocation(self->ID, uniformVarName);
    glUniform3f(location, v1, v2, v3);
}

static void setUniformV4F(shader_t *self, const char *uniformVarName, float v1, float v2, float v3, float v4)
{
    int location = getUniformLocation(self->ID, uniformVarName);
    glUniform4f(location, v1, v2, v3, v4);
}

static void setUniformM4F(shader_t *self, const char *uniformVarName, mat4 mat)
{
    int location = getUniformLocation(self->ID, uniformVarName);
    glUniformMatrix4fv(location, 1, GL_FALSE, (float *)mat);
}

shader_t *init_shader(const char *vertexShaderFilePath, const char *fragmentShaderFilePath)
{

    // 1. get data from file paths provided

    // 2. Compile the shaders
    int vertexShaderID = compile_glsl_shader(vertexShaderFilePath, GL_VERTEX_SHADER);
    int fragmentShaderID = compile_glsl_shader(fragmentShaderFilePath, GL_FRAGMENT_SHADER);

    // 3. Create the shader program
    unsigned int shaderProgramID = glCreateProgram();

    // 4. Attach shaders to program
    glAttachShader(shaderProgramID, vertexShaderID);
    glAttachShader(shaderProgramID, fragmentShaderID);

    // 5. Link shaders within program
    glLinkProgram(shaderProgramID);

    // 6. Check whether process was successful
    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        const char *message = str_concat("Shader compilation failed\n", infoLog);
        error(message);
    }

    // 7. cleanup shaders
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // 8. create struct
    shader_t *s = (shader_t *)malloc(sizeof(shader_t));

    if (s == NULL)
    {
        error("Failed to instantiate shader.");
    }

    // 9. assign function pointers
    s->ID = shaderProgramID;
    s->use = use;
    s->setUniformBool = setUniformBool;
    s->setUniformFloat = setUniformFloat;
    s->setUniformInt = setUniformInt;
    s->setUniformV2F = setUniformV2F;
    s->setUniformV3F = setUniformV3F;
    s->setUniformV4F = setUniformV4F;
    s->setUniformM4F = setUniformM4F;

    return s;
}

void destroy_shader(shader_t *self)
{
    free(self);
}