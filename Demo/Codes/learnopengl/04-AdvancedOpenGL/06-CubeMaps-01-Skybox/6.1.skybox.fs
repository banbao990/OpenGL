#version 460 core
out vec4 FragColor;

in vec3 TexCoords; // 采样坐标为 vec3(只需要方向即可)

uniform samplerCube skybox; // 采样器为 samplerCube

void main() {
    FragColor = texture(skybox, TexCoords);
}