#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 Normal;
} vs_out;

void main() {
    // ������ɫ�����ܵ�λ�������Ǽ��ÿռ�����
    // ��������Ӧ�ý��������任����ͬ�Ŀռ���
    // ����ͶӰ�任�����еĵ�3ά�Ѿ��������Ϣ(������z), ���ߵı任���ܹ��򵥵�(PVM)
    // �ҵ��뷨���ڹ۲�����ϵ�м����ԭʼ�ĵ�ͷ�����(��ʱ��ȷ)
    // Ȼ���ʱ���������������������, ��ʹ��ͶӰ�任����
    
    // ������ģ������ϵ/��������ϵ�н���
    // ���Բο�����任�����Ƶ�����
    
    // �Ѽ���ŵ� geometry shader ����
    gl_Position = vec4(aPos, 1.0);
    vs_out.Normal = normalize(aNormal);
}