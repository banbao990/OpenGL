#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

// declare an interface block
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // 世界坐标系位置
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal; // 法向变换矩阵
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // 构建 TBN 矩阵
    // 注意, 如果我们希望更精确的话就不要将TBN向量乘以model矩阵, 而是使用法线矩阵
    // 但我们只关心向量的方向, 不会平移也和缩放这个变换
    mat4 normalModel = transpose(inverse(model));
    // mat4 normalModel = model;
    vec3 T = normalize(vec3(normalModel * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(normalModel * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(normalModel * vec4(aNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    // 怎么使用 TBN 矩阵
    // 我们直接使用TBN矩阵, 这个矩阵可以把切线坐标空间的向量转换到世界坐标空间
    // 因此我们把它传给片段着色器中 
    // 把通过采样得到的法线坐标左乘上 TBN 矩阵转换到世界坐标空间中
    // 这样所有法线和其他光照变量就在同一个坐标系中了
    vs_out.TBN = TBN;
}