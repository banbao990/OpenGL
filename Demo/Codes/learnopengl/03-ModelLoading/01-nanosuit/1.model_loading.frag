#version 460 core
out vec4 FragColor;

in vec2 TexCoords; // 纹理
in vec3 normal; // 法线
in vec3 fragPos; // 观察坐标系中的坐标

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightPos; // 光源位置

void main() {
    // 计算光的方向向量
    vec3 lightDir = normalize(lightPos - fragPos);

    // 漫反射
    vec3 diffuse = vec3(texture(texture_diffuse1, TexCoords));
    // Phong 模型: 漫反射 Lamber 定律
    // dP = N dot L
    // 乘机为负数表示光照照不到(但不是阴影)
    float diff = max(dot(normal, lightDir), 0.0);
    diffuse = (diff + 0.5) * diffuse; // 加点环境光

    // 镜面高光
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(vec3(0) - fragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = spec * vec3(texture(texture_specular1, TexCoords));

    FragColor = vec4((diffuse + specular), 1.0);
}