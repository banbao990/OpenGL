#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fColor;

uniform vec2 offsets[100];

void main() {
    gl_Position = vec4(aPos+offsets[gl_InstanceID], 0.0f, 1.0f);
    fColor = aColor;
}