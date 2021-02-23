#version 460 core
out vec4 FragColor;

in vec2 TexCoords; // ����
in vec3 normal; // ����
in vec3 fragPos; // �۲�����ϵ�е�����

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightPos; // ��Դλ��

void main() {
    // �����ķ�������
    vec3 lightDir = normalize(lightPos - fragPos);

    // ������
    vec3 diffuse = vec3(texture(texture_diffuse1, TexCoords));
    // Phong ģ��: ������ Lamber ����
    // dP = N dot L
    // �˻�Ϊ������ʾ�����ղ���(��������Ӱ)
    float diff = max(dot(normal, lightDir), 0.0);
    diffuse = (diff + 0.5) * diffuse; // �ӵ㻷����

    // ����߹�
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(vec3(0) - fragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = spec * vec3(texture(texture_specular1, TexCoords));

    FragColor = vec4((diffuse + specular), 1.0);
}