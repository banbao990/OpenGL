#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

// declare an interface block
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    // mat3 TBN;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    // vec3 lightPos; // �������ô���, ������Ϊ��Ҫ��ʾ
    // vec3 viewPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {

    mat4 normalModel = transpose(inverse(model));
    
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // ��������ϵλ��
    vs_out.Normal = mat3(normalModel) * aNormal; // ����任����
    vs_out.TexCoords = aTexCoords;
    // vs_out.lightPos = lightPos;
    // vs_out.viewPos = viewPos;
    
    // ���� TBN ����
    // ע��, �������ϣ������ȷ�Ļ��Ͳ�Ҫ��TBN��������model����, ����ʹ�÷��߾���
    // ������ֻ���������ķ���, ����ƽ��Ҳ����������任
    // normalModel = model;
    vec3 T = normalize(vec3(normalModel * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(normalModel * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(normalModel * vec4(aNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    // ��ôʹ�� TBN ����
    // ����Ҳ����ʹ��TBN����������
    // ���������԰���������ռ������ת������������ռ�
    // �������ʹ�������������������ձ���
    // ������ת�������߿ռ�, �������ߺ��������ձ�����һ����һ������ϵ����
    
    // ��������, �������ھ����ת��
    TBN = transpose(TBN);
    // vs_out.TBN = TBN;
    // �Ż�
    // ����TBN�����������͸�������ɫ��
    // ���ǽ����߿ռ�Ĺ�Դλ��, �۲�λ���Լ�����λ�÷��͸�������ɫ��
    // �������ǾͲ�����������ɫ������о���˷���
    // ����һ�����ѵ��Ż�, ��Ϊ������ɫ��ͨ����������ɫ�����е���
    // ��Ҳ��Ϊʲô���ַ�����һ�ָ��õ�ʵ�ַ�ʽ��ԭ��
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    gl_Position = model * vec4(aPos, 1.0);
    vs_out.TangentFragPos = TBN * vec3(gl_Position);
    gl_Position = projection * view *gl_Position;
}