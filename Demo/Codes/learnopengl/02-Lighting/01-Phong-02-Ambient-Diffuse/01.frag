#version 460 core

uniform vec3 objectColor;
uniform vec3 lightColor;
// ��̬���ù�Դλ��(���Դ)
uniform vec3 lightPos;

in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

void main() {
    // ��׼��
    vec3 norm = normalize(normal);
    // ��ķ�������(��ָ���Դ)
    vec3 lightDir = normalize(lightPos - fragPos);
    // Phong ģ��: ������ Lamber ����
    // dP = N dot L
    // �˻�Ϊ������ʾ�����ղ���(��������Ӱ)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // ������ϵ��
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse + ambient
    vec3 result = (ambient + diffuse)* objectColor;
    fragColor = vec4(result, 1.0);
}