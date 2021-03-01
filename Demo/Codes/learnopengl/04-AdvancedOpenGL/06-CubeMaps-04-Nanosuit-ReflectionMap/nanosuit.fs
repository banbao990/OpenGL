#version 460 core
out vec4 FragColor;

in vec3 Normal; // 世界坐标系法向
in vec3 Position; // 世界坐标系位置
in vec2 TexCoords; // 纹理坐标

uniform vec3 cameraPos; // (视点)相机位置
uniform samplerCube skybox; // 天空盒
uniform sampler2D texture_diffuse1; // 漫反射贴图
uniform sampler2D texture_reflection1; // 反射贴图
uniform sampler2D texture_specular1; // 镜面高光
uniform vec3 lightPos; // 光源位置

void main() {
    // 视线方向 
    vec3 viewDir = normalize(Position - cameraPos);
    // 光线方向(指向光)
    vec3 lightDir = normalize(lightPos - Position);

    // 反射贴图
    // 我认为要反映强度, 应该是张 3 通道的反射强度
    vec3 reflectionStrength = texture(texture_reflection1, TexCoords).rgb; 
    vec3 R = reflect(viewDir, normalize(Normal));
    
    // 漫反射强度
    vec3 diffuse = texture(texture_diffuse1, TexCoords).rgb;
    // Phong 模型: 漫反射 Lamber 定律
    // dP = N dot L
    // 乘机为负数表示光照照不到(但不是阴影)
    float diff = max(dot(Normal, lightDir), 0.0);
    diffuse = diff * diffuse;

    // 镜面高光
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = spec * texture(texture_specular1, TexCoords).rgb;
    
    FragColor = vec4(
        reflectionStrength*texture(skybox, R).rgb + 
        diffuse + specular,
        1.0
    );
}