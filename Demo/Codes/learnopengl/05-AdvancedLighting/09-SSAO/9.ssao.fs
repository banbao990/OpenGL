#version 460 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
// 屏幕的平铺噪声纹理会根据屏幕分辨率除以噪声大小的值来决定
// 我们想要将噪声纹理平铺(Tile)在屏幕上
// 但是由于TexCoords的取值在0.0和1.0之间, texNoise纹理将不会平铺
// 所以我们将通过屏幕分辨率除以噪声纹理大小的方式计算TexCoords的缩放大小
// 并在之后提取相关输入向量的时候使用
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); // 放大倍数

uniform mat4 projection;

void main() {
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    
    // create TBN change-of-basis matrix: from tangent-space to view-space
    // 施密特正交化
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i) {
        // get sample position
        // 切线空间变换到观察坐标系空间
        
        // 因为 OpenGL 是列式存储, 列式计算的
        // 二维坐标的齐次坐标经过如下变换
        //   mat3(1,0,0,0,1,0,1,0,1)
        //     =>
        //      1,0,1     
        //      0,1,0     
        //      0,0,1     
        //   效果是向 x 方向平移了 1 个单位长度
        // 这里的 TBN 左乘等价于我们使用矩阵的 TBN^T 左乘
        //   而正交矩阵 TBN^T = TBN^-1
        //   所以这是正确的
        // uvn 坐标系的三个单位向量在 xyz 坐标系中表示为 (ux,uy,uz)...
        // 从 xyz 到 uvn 的正交变换为
        //      ux,uy,uz
        //      vx,vy,vz
        //      nx,ny,nz
        
        vec3 samplePos = TBN * samples[i]; 
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        // 转换到投影 -> 屏幕空间
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        // 观察坐标系中的深度
        // (采样的点是最终的投影裁剪之后的, G-Buffer 全是呀, 保存的值是观察坐标系位置)
        float sampleDepth = texture(gPosition, offset.xy).z; 
        
        // range check & accumulate
        // 当检测一个靠近表面边缘的片段时, 它将会考虑测试表面之下的表面的深度值
        // 这些值将会(不正确地)影响遮蔽因子
        // 我们可以通过引入一个范围检测从而解决这个问题
        // smoothstep 函数: 非常光滑地在第一和第二个参数范围内插值了第三个参数
            // genType smoothstep(genType edge0, genType edge1, genType x)  
            // 等价于
            // genType t;
            // t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0); // 取中间值
            // return (3.0 - 2.0 * t) * t * t;
        
        // range check (为什么需要看 README 的图)
        // 和一个规定好的值(半径 radius)比较
        // fragPos.z - sampleDepth <= radius(需要计算)
        //      => 1.0
        // fragPos.z - sampleDepth > radius(影响变小)
        //      => 1.0-0.0
        // smoothstep(x, y, z)
            // z>=y => y
            // z<y  => 插值
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}