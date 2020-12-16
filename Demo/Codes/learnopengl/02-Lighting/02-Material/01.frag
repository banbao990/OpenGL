#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

out vec4 fragColor;

void main() {

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  light.diffuse * (diff * material.diffuse);

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // diffuse + ambient + specular
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}