#version 460 core

struct Material {
    // �Ƴ��˻����������ɫ����, ��Ϊ��������ɫ�ڼ�����������¶�������������ɫ
    // vec3 ambient;
    // vec3 diffuse;
    sampler2D diffuse;
    sampler2D specular;
    // sampler2D emission;
    float shininess;
};

// һ����Դ������ ambient, diffuse, specular �������Ų�ͬ��ǿ��
struct Light {
    vec3 position;
    vec3 direction; // ƽ�й�
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // ʵ�ֹ��˥��
    // F_att = 1.0 / (Kc + Kl*d + Kq*d^2)
    float constant;
    float linear;
    float quadratic;

    // spot light
    float inCutOff;
    float outCutOff;
};

uniform Light light;
uniform Material material;

// ���(�ӵ�)λ��, �۲�����ϵ��Ϊ (0,0,0)
//uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords; // �������������

out vec4 fragColor;

void main() {

    // ˥������
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    
    // �۹�Ч��
    float theta = dot(lightDir, normalize(-light.direction));
    
    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse =  light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

    // ambient
    // vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(vec3(0) - fragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

    // emisssion
    // vec3 emission = vec3(texture(material.emission, texCoords));

    // diffuse + ambient + specular
    vec3 result = ambient + diffuse + specular /* + emission */;
    // ���ǿ��Խ�������������ֲ���, �û������ղ������ž������
    // �����������ʹ�ö���һ���Ĺ�Դ, ���еĻ�����������Ὺʼ����
    // �������������������Ҳϣ��˥����������
    result *= attenuation;

    // �������ֵ
    float rate = 1.0;
    if(theta < light.outCutOff) {
        rate = 0.0;
    } else if(theta < light.inCutOff) {
        rate = (theta-light.outCutOff)/(light.inCutOff-light.outCutOff);
    }

    fragColor = vec4(rate*result, 1.0);
}