#version 460 core

struct Material {
    // 移除了环境光材质颜色向量, 因为环境光颜色在几乎所有情况下都等于漫反射颜色
    // vec3 ambient;
    // vec3 diffuse;
    sampler2D diffuse;
    sampler2D specular;
    // sampler2D emission;
    float shininess;
};

// 一个光源对它的 ambient, diffuse, specular 光照有着不同的强度
struct Light {
    vec3 position;
    vec3 direction; // 平行光
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // 实现光的衰减
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

// 相机(视点)位置, 观察坐标系中为 (0,0,0)
//uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords; // 输入的纹理坐标

out vec4 fragColor;

void main() {

    // 衰减计算
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 聚光效果
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
    // 我们可以将环境光分量保持不变, 让环境光照不会随着距离减少
    // 但是如果我们使用多于一个的光源, 所有的环境光分量将会开始叠加
    // 所以在这种情况下我们也希望衰减环境光照
    result *= attenuation;

    // 计算过渡值
    float rate = 1.0;
    if(theta < light.outCutOff) {
        rate = 0.0;
    } else if(theta < light.inCutOff) {
        rate = (theta-light.outCutOff)/(light.inCutOff-light.outCutOff);
    }

    fragColor = vec4(rate*result, 1.0);
}