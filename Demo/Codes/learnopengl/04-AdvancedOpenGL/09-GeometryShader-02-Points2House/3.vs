#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

// ����û�����õĽӿڿ�, ��Ҫ�Լ�����

// �ӿڿ��ڼ�����ɫ����������ɫ���л�����״���һ��
// ʵ����, ������ɫ���������ܹ���÷ǳ���
// �����Ǻϲ�Ϊһ����Ľӿڿ������������߼�һ��

out VS_OUT {
    vec3 color;
} vs_out;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.color = aColor;
}