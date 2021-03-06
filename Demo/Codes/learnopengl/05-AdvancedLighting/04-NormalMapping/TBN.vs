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
    mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // ��������ϵλ��
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal; // ����任����
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // ���� TBN ����
    // ע��, �������ϣ������ȷ�Ļ��Ͳ�Ҫ��TBN��������model����, ����ʹ�÷��߾���
    // ������ֻ���������ķ���, ����ƽ��Ҳ����������任
    mat4 normalModel = transpose(inverse(model));
    // mat4 normalModel = model;
    vec3 T = normalize(vec3(normalModel * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(normalModel * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(normalModel * vec4(aNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    // ��ôʹ�� TBN ����
    // ����ֱ��ʹ��TBN����, ���������԰���������ռ������ת������������ռ�
    // ������ǰ�������Ƭ����ɫ���� 
    // ��ͨ�������õ��ķ������������ TBN ����ת������������ռ���
    // �������з��ߺ��������ձ�������ͬһ������ϵ����
    vs_out.TBN = TBN;
}