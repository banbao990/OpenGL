#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform bool channelA;

void main() {
    vec3 color;
    if(channelA) {
        color = vec3(texture(scene, TexCoords).a);
    } else {
        color = texture(scene, TexCoords).rgb;
    }
    FragColor = vec4(color, 1.0);
}