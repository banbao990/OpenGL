#version 460 core

struct Material {
    // �Ƴ��˻����������ɫ����, ��Ϊ��������ɫ�ڼ�����������¶�������������ɫ
    // vec3 ambient;
    // vec3 diffuse;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

// һ����Դ������ ambient, diffuse, specular �������Ų�ͬ��ǿ��
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;

// ���(�ӵ�)λ��, �۲�����ϵ��Ϊ (0,0,0)
uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords; // �������������

out vec4 fragColor;

void main() {

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse =  light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

    // ambient
    // vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

    // diffuse + ambient + specular
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}