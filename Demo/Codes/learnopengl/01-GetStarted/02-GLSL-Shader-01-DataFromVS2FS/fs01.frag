#version 460 core

// Ƭ����ɫ�����������
out vec4 FragColor;
// �Ӷ�����ɫ���������������(������ͬ && ������ͬ)
in vec4 vertexColor; 

void main() {
    FragColor = vertexColor;
}