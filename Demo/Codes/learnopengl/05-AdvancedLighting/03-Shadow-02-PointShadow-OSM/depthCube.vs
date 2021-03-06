#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main() {
    // 切换到世界坐标系中
    gl_Position = model * vec4(aPos, 1.0);
}