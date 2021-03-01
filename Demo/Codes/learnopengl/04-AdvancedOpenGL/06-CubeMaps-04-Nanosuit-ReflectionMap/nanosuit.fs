#version 460 core
out vec4 FragColor;

in vec3 Normal; // ��������ϵ����
in vec3 Position; // ��������ϵλ��
in vec2 TexCoords; // ��������

uniform vec3 cameraPos; // (�ӵ�)���λ��
uniform samplerCube skybox; // ��պ�
uniform sampler2D texture_diffuse1; // ��������ͼ
uniform sampler2D texture_reflection1; // ������ͼ
uniform sampler2D texture_specular1; // ����߹�
uniform vec3 lightPos; // ��Դλ��

void main() {
    // ���߷��� 
    vec3 viewDir = normalize(Position - cameraPos);
    // ���߷���(ָ���)
    vec3 lightDir = normalize(lightPos - Position);

    // ������ͼ
    // ����ΪҪ��ӳǿ��, Ӧ������ 3 ͨ���ķ���ǿ��
    vec3 reflectionStrength = texture(texture_reflection1, TexCoords).rgb; 
    vec3 R = reflect(viewDir, normalize(Normal));
    
    // ������ǿ��
    vec3 diffuse = texture(texture_diffuse1, TexCoords).rgb;
    // Phong ģ��: ������ Lamber ����
    // dP = N dot L
    // �˻�Ϊ������ʾ�����ղ���(��������Ӱ)
    float diff = max(dot(Normal, lightDir), 0.0);
    diffuse = diff * diffuse;

    // ����߹�
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = spec * texture(texture_specular1, TexCoords).rgb;
    
    FragColor = vec4(
        reflectionStrength*texture(skybox, R).rgb + 
        diffuse + specular,
        1.0
    );
}