#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec3 fColor;

void main() {
    // т╡пн
    float a = abs((gl_InstanceID / 10) - 4.5);
    float b = abs((gl_InstanceID % 10) - 4.5);
    vec2 pos = aPos * ((a*a + b*b)/ 100.0);
    gl_Position = vec4(pos + aOffset, 0.0, 1.0);
    fColor = aColor;
}