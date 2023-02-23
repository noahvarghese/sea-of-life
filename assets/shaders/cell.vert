#version 460 core

layout (location = 0) in vec2 point;

uniform bool accelerate;
uniform vec3 cell;
uniform vec2 gridCenter;

uniform float cellWidth = 10.0;
uniform vec3 aliveColor = vec3(1.0, 1.0, 1.0);
uniform vec3 deadColor = vec3(0.3, 0.3, 0.3);

uniform mat4 projection = mat4(
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

uniform mat4 model = mat4(
    vec4(1.0, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
);

out vec4 color;

void main()
{
    vec4 position = vec4(point, 1.0, 1.0);

    if (accelerate) {
        // Matrices are column major ordered...
        mat4 translatedView = mat4(
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(cell.y - gridCenter.y, cell.x - gridCenter.x, -1.0, 1.0)
        );
        
        gl_Position = projection * translatedView * model * position;
    } else {
        gl_Position = projection * view * model * position;
    }

    gl_PointSize = cellWidth;

    if (cell.z == 1.0) {
        color = vec4(aliveColor, 1.0);
    } else{
        color = vec4(deadColor, 1.0);
    }
}