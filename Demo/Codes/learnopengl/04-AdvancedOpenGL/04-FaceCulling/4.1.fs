#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1; // 具体是哪一张纹理图

void main() {             
    FragColor = texture(texture1, TexCoords);
}