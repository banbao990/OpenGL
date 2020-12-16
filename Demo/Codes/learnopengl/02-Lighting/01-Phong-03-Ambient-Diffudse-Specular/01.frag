#version 460 core

uniform vec3 objectColor;
uniform vec3 lightColor;
// 静态设置光源位置(点光源)
uniform vec3 lightPos;
// 相机(视点)位置
uniform vec3 viewPos;
// 环境光强度
uniform float ambientStrength;
// 高光强度
uniform float specularStrength;
// 高光的散射程度(越大散射越不明显, 光照越集中)
uniform int shininess;

in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

void main() {
    // 标准化
    vec3 norm = normalize(normal);
    // 光的方向向量(点指向光源)
    vec3 lightDir = normalize(lightPos - fragPos);
    // Phong 模型: 漫反射 Lamber 定律
    // dP = N dot L
    // 乘机为负数表示光照照不到(但不是阴影)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 环境光系数
    vec3 ambient = ambientStrength * lightColor;

    // 高光
    // N 法线 norm(点->射出)
    // L 入射光: lightDir, 点->光源
    // R 反射光: reflectDir, 点->射出
    // R = 2 N (L dot N) - L
    // 视线: viewDir, 点-> 视点

    // 调用函数实现, reflect
    // I: Specifies the incident vector.
    // N: Specifies the normal vector.
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(viewPos - fragPos);
    // pow(x, shininess), 0<=x<=1, shininess 越大越凹
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // diffuse + ambient
    vec3 result = (ambient + diffuse + specular)* objectColor;
    fragColor = vec4(result, 1.0);
}