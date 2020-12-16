#version 460 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    // ���ϻ�����ϵ��
    float ambientStrength = 0.1;
    FragColor = vec4(ambientStrength * lightColor * objectColor, 1.0);
}