#version 460 core
out vec4 FragColor;

in vec3 Normal; // ��������ϵ����
in vec3 Position; // ��������ϵλ��
in vec2 TexCoords; // ��������

uniform vec3 cameraPos; // (�ӵ�)���λ��
uniform sampler2D texture_diffuse1; // ��������ͼ
uniform sampler2D texture_specular1; // ����߹�
uniform vec3 lightPos; // ��Դλ��

void main() {
    // ���߷��� 
    vec3 viewDir = normalize(Position - cameraPos);
    // ���߷���(ָ���)
    vec3 lightDir = normalize(lightPos - Position);
    
    // ������ǿ��
    vec3 diffuse = vec3(texture(texture_diffuse1, TexCoords));
    // Phong ģ��: ������ Lamber ����
    // dP = N dot L
    // �˻�Ϊ������ʾ�����ղ���(��������Ӱ)
    float diff = max(dot(Normal, lightDir), 0.0);
    diffuse = (diff+0.5) * diffuse; // ������

    // ����߹�
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = spec * vec3(texture(texture_specular1, TexCoords));
    
    FragColor = vec4(
        diffuse + specular,
        1.0
    );
}