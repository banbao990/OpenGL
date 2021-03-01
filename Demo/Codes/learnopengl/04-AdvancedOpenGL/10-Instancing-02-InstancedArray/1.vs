#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec3 fColor;

void main() {
    gl_Position = vec4(aPos+aOffset, 0.0f, 1.0f);
    fColor = aColor;
}