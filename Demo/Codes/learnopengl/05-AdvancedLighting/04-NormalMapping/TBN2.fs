#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    // vec3 lightPos; // 正常不用传递, 这里因为需要演示
    // vec3 viewPos;
} fs_in;

uniform sampler2D diffuseTexture; // 图片纹理
uniform sampler2D normalTexture; // 法线纹理
uniform bool blinn;
uniform bool normalMappingOn;
uniform int onlyAmbient; // 只开启环境光
uniform vec3 lightPos; // 可以直接传递给两个 shader
uniform vec3 viewPos;

void main() {
    // TBN
    // TODO

    vec3 lightDir;
    vec3 viewDir;
    vec3 normal;

    if(normalMappingOn) {
        normal = normalize(2.0 * texture(normalTexture, fs_in.TexCoords).rgb - 1.0);
        lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
        viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    } else {
        normal = normalize(fs_in.Normal);
        lightDir = normalize(lightPos - fs_in.FragPos);
        viewDir = normalize(viewPos - fs_in.FragPos);
    }

    // 纹理颜色
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;

    // 环境光 ambient
    vec3 ambient = 0.3 * color;

    // 漫发射光 diffuse
    float diff = max(dot(lightDir, normal), 0.0); // diff = N \cdot L
    vec3 diffuse = diff * color;

    // 镜面高光 specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn) {
        // Blinn-Phong 模型考虑法线和半角矢量的夹角
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    } else {
        // Phong 模型考虑反射光线和视线的夹角
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = spec * color;
    // 阴影不影响环境光
    // assuming bright white light color
    FragColor = vec4(ambient + (1-onlyAmbient)*(diffuse + specular)*vec3(0.3), 1.0);
}


// a^Ta = 1, M^T = M^{-1}
// =>
// (Ma)^T(Ma) = a^TM^TMa = a^T(M^TM)a = a^T(M^{-1}M)a = a^Ta=1