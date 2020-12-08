#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixRate;

void main() {
    FragColor = 
        // mixRate*texture2 + (1-mixRate)*texture1
        mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixRate);
}