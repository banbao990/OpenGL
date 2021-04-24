#version 460 core

// ָ����Ⱦ��������һ����ɫ����
layout (location = 0) out vec3 gPosition; // λ�û���
layout (location = 1) out vec3 gNormal;   // ���򻺳�
layout (location = 2) out vec3 gFlux;     // ͨ������

in vec3 fColor;  // ��ɫ
in vec3 wPos;    // ��������ϵλ��
in vec3 wNormal; // ��������ϵ����

uniform vec3 lightPos;

void main() {
    // ������������ϵλ��
    gPosition = wPos;
    
    // ������������ϵ����
    gNormal = wNormal;
    
    // ����ͨ��������
    // ���������(Phong)
    // ��Ϊ��Դ�ǰ�ɫ
    vec3 lightDir = normalize(lightPos - wPos);
    float diff = max(dot(lightDir, gNormal), 0.0);
    gFlux = vec3(diff * fColor);
}