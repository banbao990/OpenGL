#version 460 core

out vec4 FragColor;

// 采样器
uniform sampler2D gPosition;   // 位置(LightSpace)
uniform sampler2D gNormal;     // 法向(LightSpace)
uniform sampler2D gFlux;       // 通量(LightSpace)
uniform sampler2D gDepthMap;   // 深度(LightSpace)
uniform sampler2D gRandomMap;  // 随机采样的移动
uniform bool rsmOn;                 // 是否开启 RSM
uniform vec3 lightPos;              // 光源位置(世界坐标系)
uniform int randomSamples;          // 随机采样样本个数
uniform vec3 viewPos;               // 视点位置(世界坐标系)
uniform float radius;               // 采样半径
uniform float indirectStrength;     // 间接光照强度

in vec3 fColor;      // 颜色
in vec3 fPosLight;   // 以光源为视点的坐标系中的位置
in vec3 fPos;        // 世界坐标系中的位置
in vec3 fNormal;     // 世界坐标系中的法向


// 解决黑白条纹阴影问题
// 最好先经过线性化之后再做比较, 否则值不好控制
const float depthBias = 0.0005f;

// 计算间接光照
// normal 单位化的法向量
vec3 calcIndirectLight(vec3 normal) {
    vec3 indirectLight = vec3(0.0f);
    for(int i = 0;i < randomSamples; ++i) {
        // texture 和 texelFetch 的区别
        //   texture: 归一化到 [0, 1], 经过 blur
        //   texelFetch: 原始坐标空间, 没有经过 blur
        
        // 采样偏移
        vec3 offset = texelFetch(gRandomMap, ivec2(i, 0), 0).xyz;
        // 采样点在以光源为视点的观察坐标系中的位置(用于采样)
        vec2 samplePosInLightSpace = fPosLight.xy + radius*offset.xy;

        // 采样点在世界坐标系中的位置、法向、光通量
        vec3 xp = texture(gPosition, samplePosInLightSpace).xyz;
        vec3 np = normalize(texture(gNormal, samplePosInLightSpace).xyz); // blur 引入非单位化
        vec3 fp = texture(gFlux, samplePosInLightSpace).xyz;
        
        vec3 xMinusxp = normalize(fPos - xp);
        // 计算间接光照
        indirectLight += fp 
            * offset.z
            * max(dot(np, xMinusxp), 0.0f)
            * max(dot(normal, -xMinusxp), 0.0f)
            / pow(length(xMinusxp), 4.0f);
    }
    return clamp(
        indirectLight/randomSamples *40* indirectStrength, 
        0.0f, 1.0f);
}

// 处于阴影之中: 0.0
// 否则返回 1.0
// GLSL 函数参数都是按值传递的
float ShadowCalculation(vec3 projCoords) {
    // xyz:[-1, 1] => [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    // 在深度图中获取深度信息
    float closestDepth = texture(gDepthMap, projCoords.xy).r;
    // 判断是否处于阴影当中
    // Bias 解决阴影的条纹问题
    
    // 多次采样取平均 PCF, 变化更加柔和
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(gDepthMap, 0); // 0 级纹理相邻纹素的距离
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(gDepthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += ((projCoords.z - depthBias)> pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // 远处始终可见
    if(projCoords.z > 1.0f) {
        shadow = 0.0f;
    }
    return 1.0f - shadow;
}


void main() {
    // 计算是否处在阴影之中
    float notInShadow = ShadowCalculation(fPosLight);

    // (1) 直接光照
    // 认为光源为白光

    // 环境光 ambient
    vec3 ambient = 0.05 * fColor;

    // 漫发射光 diffuse
    vec3 lightDir = normalize(lightPos - fPos);
    // 归一化, fs 中归一化可能在插值的过程中变成非单位向量
    vec3 normal = normalize(fNormal);
    float diff = max(dot(lightDir, normal), 0.0f); // diff = N \cdot L
    vec3 diffuse = diff * fColor;

    // 镜面高光 specular
    vec3 viewDir = normalize(viewPos - fPos);
    // Blinn-Phong 模型考虑法线和半角矢量的夹角
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f); // 32.0 为光照镜面反射系数 shiness
    vec3 specular = vec3(1.0f) * spec; // 1.0f 为亮度调整系数

    // (2) 间接光照
    vec3 indirect = vec3(0.0f);
    if(rsmOn) {
        indirect = calcIndirectLight(normal);
    }
    // 阴影不影响间接光和环境光
    FragColor = vec4(
        (diffuse + specular)*notInShadow + ambient + indirect
        , 1.0f);
    // 伽马校正
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));
}