#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = aPos; // 原点在中心, 因此可以直接把纹理坐标用位置坐标代替
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // 这样子的设置将把天空盒的深度始终设置为 1.0
}