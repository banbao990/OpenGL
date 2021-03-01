#version 460 core
out vec4 FragColor;

in vec3 Normal; // 世界坐标系法向
in vec3 Position; // 世界坐标系位置
in vec2 TexCoords; // 纹理坐标

uniform vec3 cameraPos; // (视点)相机位置
uniform sampler2D texture_diffuse1; // 漫反射贴图
uniform sampler2D texture_specular1; // 镜面高光
uniform vec3 lightPos; // 光源位置

void main() {
    // 视线方向 
    vec3 viewDir = normalize(Position - cameraPos);
    // 光线方向(指向光)
    vec3 lightDir = normalize(lightPos - Position);
    
    // 漫反射强度
    vec3 diffuse = vec3(texture(texture_diffuse1, TexCoords));
    // Phong 模型: 漫反射 Lamber 定律
    // dP = N dot L
    // 乘机为负数表示光照照不到(但不是阴影)
    float diff = max(dot(Normal, lightDir), 0.0);
    diffuse = (diff+0.5) * diffuse; // 环境光

    // 镜面高光
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = spec * vec3(texture(texture_specular1, TexCoords));
    
    FragColor = vec4(
        diffuse + specular,
        1.0
    );
}