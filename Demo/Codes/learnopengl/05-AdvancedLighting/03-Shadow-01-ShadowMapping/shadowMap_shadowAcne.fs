#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace; // 以光源为视点的观察坐标系中点的位置
} fs_in;

uniform sampler2D floorTexture; // 图片纹理
uniform sampler2D shadowMap; // 深度图
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

// bias = 0.005f 写死, 可能有极端情况无法处理

// 处于阴影之中: 1.0
// 否则返回 0.0
float ShadowCalculation(vec4 fragPosLightSpace) {
    
    // 转化为标准齐次坐标, z:[-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // xyz:[-1, 1] => [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    
    // 在深度图中获取深度信息
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // 当前片元的深度
    float currentDepth = projCoords.z;
    
    // 判断是否处于阴影当中
    // 添加一个 bias 解决 shadow acne 失真问题
    float bias = 0.005;
    float shadow = (currentDepth-bias) > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main() {
    // 计算是否处于阴影之中
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);  

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
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    // 阴影不影响环境光
    FragColor = vec4(ambient + (1-shadow)*(diffuse + specular), 1.0);
}

// Phong 模型的问题
// 在镜面高光区域的边缘出现了一道很明显的断层
// 出现这个问题的原因是观察向量和反射向量间的夹角不能大于90度
// 如果点积的结果为负数, 镜面光分量会变为0.0
// 你可能会觉得, 当光线与视线夹角大于 90 度时你应该不会接收到任何光才对, 所以这不是什么问题
// 然而, 这种想法仅仅只适用于漫反射分量
// 当考虑漫反射光的时候, 如果法线和光源夹角大于90度, 光源会处于被照表面的下方
// 这个时候光照的漫反射分量的确是为 0.0
// 但是, 在考虑镜面高光时, 我们测量的角度并不是光源与法线的夹角, 而是视线与反射光线向量的夹角

// 问题出在当物体的反光度非常小时
// 它产生的镜面高光半径足以让这些相反方向的光线对亮度产生足够大的影响
// 在这种情况下就不能忽略它们对镜面光分量的贡献

// 半角矢量的引入使得不论观察者向哪个方向看
// 半程向量与表面法线之间的夹角都不会超过90度(除非光源在表面以下)

// 一些区别
// 半程向量与表面法线的夹角通常会小于观察与反射向量的夹角
// 如果你想获得和冯氏着色类似的效果
// 就必须在使用 Blinn-Phong 模型时将镜面反光度设置更高一点
// 通常我们会选择冯氏着色时反光度分量的 2 到 4 倍