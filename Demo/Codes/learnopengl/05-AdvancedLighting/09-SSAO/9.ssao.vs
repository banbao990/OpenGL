#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

// SSAO 遮蔽值生成, 2D 平面渲染
void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}