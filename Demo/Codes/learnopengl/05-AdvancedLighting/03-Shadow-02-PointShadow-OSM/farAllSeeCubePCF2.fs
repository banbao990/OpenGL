#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture; // 图片纹理
uniform samplerCube depthMap; // 立方体深度贴图
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;
uniform float far_plane; // 远平面位置
uniform int onlyAmbient; // 只开启环境光

// 处于阴影之中: 1.0
// 否则返回 0.0
float ShadowCalculation(vec3 fragPos) {
    // 方向向量
    vec3 fragToLight = fragPos - lightPos;
    // 计算当前点的深度
    float currentDepth = length(fragToLight);
    
    // 大多数这些采样都是多余的, 与其在原始方向向量附近处采样
    // 不如在采样方向向量的垂直方向进行采样更有意义
    // 可是，没有(简单的)方式能够指出哪一个子方向是多余的
    // 有个技巧可以使用, 用一个偏移量方向数组, 它们差不多都是分开的, 每一个指向完全不同的方向
    // 剔除彼此接近的那些子方向
    // 下面就是一个有着20个偏移方向的数组
    
    // 以光线方向为一条轴的坐标系
    vec3 sampleOffsetDirections[20] = vec3[](
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );
    
    float shadow = 0.0;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = 0.05;
    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;   // Undo mapping [0;1]
        // 远处始终可见
        if(currentDepth > closestDepth && currentDepth < far_plane) {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);
    
    // 显示 shadow map
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
    return shadow;
}

void main() {
    // 计算是否处于阴影之中
    float shadow = ShadowCalculation(fs_in.FragPos);  

    // 纹理颜色
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
    
    // 环境光 ambient
    vec3 ambient = 0.3 * color;
    
    // 漫发射光 diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0); // diff = N \cdot L
    vec3 diffuse = diff * color;
    
    // 镜面高光 specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
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
    FragColor = vec4(ambient + (1-onlyAmbient) * (1-shadow)*(diffuse + specular)*vec3(0.3), 1.0);
}