#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 fColor;

void main() {
    fColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}