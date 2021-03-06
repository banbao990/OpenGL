#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
    // �л����Թ�ԴΪ�ӵ�Ĺ۲�����ϵ��
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}