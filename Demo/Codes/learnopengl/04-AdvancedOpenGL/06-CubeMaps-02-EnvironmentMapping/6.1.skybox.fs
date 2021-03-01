#version 460 core
out vec4 FragColor;

in vec3 TexCoords; // ��������Ϊ vec3(ֻ��Ҫ���򼴿�)

uniform samplerCube skybox; // ������Ϊ samplerCube

void main() {
    FragColor = texture(skybox, TexCoords);
}