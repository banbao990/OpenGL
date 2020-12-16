#version 460 core

uniform vec3 objectColor;
uniform vec3 lightColor;
// ��̬���ù�Դλ��(���Դ)
uniform vec3 lightPos;
// ���(�ӵ�)λ��
uniform vec3 viewPos;
// ������ǿ��
uniform float ambientStrength;
// �߹�ǿ��
uniform float specularStrength;
// �߹��ɢ��̶�(Խ��ɢ��Խ������, ����Խ����)
uniform int shininess;

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
    vec3 ambient = ambientStrength * lightColor;

    // �߹�
    // N ���� norm(��->���)
    // L �����: lightDir, ��->��Դ
    // R �����: reflectDir, ��->���
    // R = 2 N (L dot N) - L
    // ����: viewDir, ��-> �ӵ�

    // ���ú���ʵ��, reflect
    // I: Specifies the incident vector.
    // N: Specifies the normal vector.
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(viewPos - fragPos);
    // pow(x, shininess), 0<=x<=1, shininess Խ��Խ��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // diffuse + ambient
    vec3 result = (ambient + diffuse + specular)* objectColor;
    fragColor = vec4(result, 1.0);
}