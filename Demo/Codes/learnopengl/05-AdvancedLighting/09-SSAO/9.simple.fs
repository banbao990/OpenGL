#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform bool useR;

void main() {
    vec3 color = texture(scene, TexCoords).rgb;
    if(useR) {
        color.g = color.r;
        color.b = color.r;
    }
    FragColor = vec4(color, 1.0);
}