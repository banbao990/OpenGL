#version 460 core
out vec4 FragColor;

// uniform ��ȫ�ֱ���
// ���Ա���ɫ�������������ɫ��������׶η���
// �������ɫ�������ݽ���

uniform vec4 ourColor;

void main() {
    FragColor = ourColor;
}