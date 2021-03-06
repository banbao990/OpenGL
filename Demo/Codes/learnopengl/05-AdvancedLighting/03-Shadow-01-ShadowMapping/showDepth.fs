#version 460 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D depthMap;

void main() {
    float depthValue = texture(depthMap, TexCoords).r;
    // orthographic ����ͶӰ�������Ե� [0, 1]
    FragColor = vec4(vec3(depthValue), 1.0); 
}