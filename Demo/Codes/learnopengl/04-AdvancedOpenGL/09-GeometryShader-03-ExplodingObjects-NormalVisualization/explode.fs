#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords; // ��������ϵ����
    vec3 Normal;    // ��������ϵλ��
    vec3 Position;  // ��������
} fs_in;

uniform vec3 cameraPos; // (�ӵ�)���λ��
uniform sampler2D texture_diffuse1; // ��������ͼ
uniform sampler2D texture_specular1; // ����߹�
uniform vec3 lightPos; // ��Դλ��

void main() {
    // ���߷��� 
    vec3 viewDir = normalize(fs_in.Position - cameraPos);
    // ���߷���(ָ���)
    vec3 lightDir = normalize(lightPos - fs_in.Position);
    
    // ������ǿ��
    vec3 diffuse = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // Phong ģ��: ������ Lamber ����
    // dP = N dot L
    // �˻�Ϊ������ʾ�����ղ���(��������Ӱ)
    float diff = max(dot(fs_in.Normal, lightDir), 0.0);
    diffuse = (diff+0.5) * diffuse; // ������

    // ����߹�
    vec3 reflectDir = reflect(-lightDir, fs_in.Normal);
    float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = spec * texture(texture_specular1, fs_in.TexCoords).rgb;
    
    FragColor = vec4(
        (diffuse + specular),
        1.0
    );
}