#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace; // 以光源为视点的观察坐标系中点的位置
} fs_in;

//uniform sampler2D floorTexture;
uniform sampler2D depthMap;
uniform sampler2D d_d2_filter;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 color;
uniform int SMType;
uniform float lightWidth;  // 光源的宽度
uniform float SMDiffuse;   // 阴影的弥散程度
uniform float PCF_SampleRadius;       // PCF 采样半径

#define BIAS 0.005
#define NEAR_PLANE 2.0
#define FAR_PLANE 20.0

#define PCF_RADIUS 6
#define BLOCK_RADIUS 5


// 输入必须是 [-1, 1] 的透视深度
// 输出为 [0, 1] 的线性深度
float getLinearizeDepth(float depth) {
    // 简单通过代入 [-1, 1] 验证结果为 [NEAR_PLANE, FAR_PLANE]
    float z = (2.0 * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - depth * (FAR_PLANE - NEAR_PLANE));
    return (z - NEAR_PLANE)/(FAR_PLANE - NEAR_PLANE);
}

// 处于阴影之中: 0.0
// 否则返回 1.0
float ShadowCalculation(vec4 fragPosLightSpace) {
    // 转化为标准齐次坐标, z:[-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // xyz:[-1, 1] => [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    // 在深度图中获取深度信息
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // 当前片元的深度
    float currentDepth = projCoords.z;
    // 判断是否处于阴影当中
    float shadow = currentDepth-BIAS > closestDepth  ? 1.0 : 0.0;
    return 1.0 - shadow;
}

// PCF 算法
float PCF(vec4 fragPosLightSpace, float radius) {
    // 采样距离修正
    radius *= PCF_SampleRadius;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float shadow = 0.0;
    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0); // 0 级纹理相邻纹素的距离
    for(int x = -PCF_RADIUS; x <= PCF_RADIUS; ++x) {
        for(int y = -PCF_RADIUS; y <= PCF_RADIUS; ++y) {
            float shadowMapDepth = texture(depthMap, projCoords.xy + radius*vec2(x, y) * texelSize).r; 
            shadow += currentDepth-BIAS > shadowMapDepth  ? 1.0 : 0.0;
        }
    }
    float total = (2*PCF_RADIUS+1);
    return 1.0 - shadow/(total*total);
}

// PCSS 算法

/**
 * uv: depthMap 中的坐标
 * zRecerver: 计算得到的线性深度值 [0, 1]
 */
float findBlocker(vec2 uv, float zReceiver) {
    int blockers = 0;
    float ret = 0.0;
    float r = lightWidth * (zReceiver - NEAR_PLANE/FAR_PLANE) / zReceiver;
    // 弥散参数修正
    r *= SMDiffuse;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0); // 0 级纹理相邻纹素的距离
    for(int x = -BLOCK_RADIUS; x <= BLOCK_RADIUS; ++x) {
        for(int y = -BLOCK_RADIUS; y <= BLOCK_RADIUS; ++y) {
            // [0, 1]
            float shadowMapDepth = texture(depthMap, uv + r*vec2(x, y) * texelSize).r;
            // [0, 1] => [-1, 1]
            shadowMapDepth = getLinearizeDepth(shadowMapDepth * 2.0 - 1.0);
            if(zReceiver - BIAS > shadowMapDepth) {
                ret += shadowMapDepth;
                ++blockers;
            }
        }
    }
    // 没有 blocker 
    if(blockers == 0) {
        return -1.0;
    }
    return ret/blockers;
}

float PCSS(vec4 fragPosLightSpace){
    
    // depthMap 中的坐标
    // => [-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 转换为线性深度
    float depth = getLinearizeDepth(projCoords.z);
    // [-1, 1] => [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    // STEP 1: avgblocker depth
    float avgDepth = findBlocker(projCoords.xy, depth);
    // 没有遮挡物
    if(avgDepth == -1.0) {
        return 1.0; 
    }

    // STEP 2: penumbra size
    float penumbra = (depth - avgDepth) / avgDepth * lightWidth;
    float filterRadius = penumbra * NEAR_PLANE / (depth);

    // STEP 3: filtering
    return PCF(fs_in.FragPosLightSpace, filterRadius);
}


// VSM 算法
float VSM(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 转换为线性深度
    float depth = getLinearizeDepth(projCoords.z);
    // [-1, 1] => [0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    vec2 d_d2 = texture(d_d2_filter, projCoords.xy).rg;
    float var = d_d2.y - d_d2.x * d_d2.x; // E(X-EX)^2 = EX^2-E^2X

    // 不满足不等式, 直接可见
    if(depth - BIAS < d_d2.x){
        return 1.0;
    }
    else{
        float t_minus_mu = depth - d_d2.x;
        return var/(var + t_minus_mu*t_minus_mu);
    }
}


void main() {
    // 环境光 ambient
    vec3 ambient = 0.2 * color;
    
    // 漫发射光 diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // 镜面高光 specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;

    // Blinn-Phong 模型考虑法线和半角矢量的夹角
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec;


    float visibility = 1.0f;
    if(SMType == 1) {
        visibility = ShadowCalculation(fs_in.FragPosLightSpace);
    } else if(SMType == 2) {
        visibility = PCF(fs_in.FragPosLightSpace, 1.0);
    } else if(SMType == 3) {
        visibility = PCSS(fs_in.FragPosLightSpace);
    } else if(SMType == 4) {
        visibility = VSM(fs_in.FragPosLightSpace);
    }
    
    FragColor = vec4(ambient + visibility*(diffuse + specular), 1.0);
}