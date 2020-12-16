#version 460 core

uniform vec3 objectColor;
uniform vec3 lightColor;
// 静态设置光源位置(点光源)
uniform vec3 lightPos;

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
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse + ambient
    vec3 result = (ambient + diffuse)* objectColor;
    fragColor = vec4(result, 1.0);
}