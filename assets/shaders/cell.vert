#version 460 core

layout (location = 0) in vec3 aPosition;

uniform mat4 model = mat4(
    vec4(1.0, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
);

uniform mat4 view = mat4(
    vec4(1.0, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
);

uniform mat4 projection = mat4(
    vec4(1.0, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
);

out vec4 color;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);

    if (is_alive == 1) {
        color = vec4(aliveColor.x, aliveColor.y, aliveColor.z, 1.0);
    } else {
        color = vec4(deadColor, 1.0);
    }
}