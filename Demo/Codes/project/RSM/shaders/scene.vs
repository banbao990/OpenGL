#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec3 fColor;  // ��ɫ
out vec3 wPos;    // ��������ϵλ��
out vec3 wNormal; // ��������ϵ����

void main() {
    fColor = aColor;
    wPos = vec3(model * vec4(aPos, 1.0f)); // �� 4 άһ��Ϊ 1.0
    wNormal = mat3(transpose(inverse(model))) * aNormal;
    
    // �Թ�ԴΪ�ӵ�Ĺ۲�����ϵ
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0f);
}