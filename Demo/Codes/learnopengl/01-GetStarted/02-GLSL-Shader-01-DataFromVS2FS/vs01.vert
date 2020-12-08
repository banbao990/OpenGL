// ����: �����ת��Ϊ����ɫ

// ��ɫ��֮���໥����, ֻ��ͨ�����������ͨ��

// ��ͷ:�汾��, ����ģʽ
#version 460 core

// ��������: vecn, bvecn, ivecn, uvecn, dvecn

// ���Ĵ���ʽ
// vec2.x, vec2.y, vec2.xy
// vec3 = vec2.xxy + vec2.xyy

// ���캯��
// vec2(1.0, 1.0)
// vec3(vec2, 1.0)

// ���������
// ������ɫ������/���
// �Ӷ���������ֱ�ӽ�������, ������������� location

// location λ�ñ���������λ��ֵΪ 0
layout (location = 0) in vec3 aPos;

// ΪƬ����ɫ��ָ��һ����ɫ���
out vec4 vertexColor; 
void main() {
    // ע��������ΰ�һ�� vec3 ��Ϊ vec4 �Ĺ������Ĳ���
    gl_Position = vec4(aPos, 1.0); 
    // �������������Ϊ����ɫ
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); 
}