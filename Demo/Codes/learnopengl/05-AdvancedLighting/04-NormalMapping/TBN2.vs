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
    // mat3 TBN;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    // vec3 lightPos; // 正常不用传递, 这里因为需要演示
    // vec3 viewPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {

    mat4 normalModel = transpose(inverse(model));
    
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // 世界坐标系位置
    vs_out.Normal = mat3(normalModel) * aNormal; // 法向变换矩阵
    vs_out.TexCoords = aTexCoords;
    // vs_out.lightPos = lightPos;
    // vs_out.viewPos = viewPos;
    
    // 构建 TBN 矩阵
    // 注意, 如果我们希望更精确的话就不要将TBN向量乘以model矩阵, 而是使用法线矩阵
    // 但我们只关心向量的方向, 不会平移也和缩放这个变换
    // normalModel = model;
    vec3 T = normalize(vec3(normalModel * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(normalModel * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(normalModel * vec4(aNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    // 怎么使用 TBN 矩阵
    // 我们也可以使用TBN矩阵的逆矩阵
    // 这个矩阵可以把世界坐标空间的向量转换到切线坐标空间
    // 因此我们使用这个矩阵左乘其他光照变量
    // 把他们转换到切线空间, 这样法线和其他光照变量再一次在一个坐标系中了
    
    // 正交矩阵, 逆矩阵等于矩阵的转置
    TBN = transpose(TBN);
    // vs_out.TBN = TBN;
    // 优化
    // 不把TBN矩阵的逆矩阵发送给像素着色器
    // 而是将切线空间的光源位置, 观察位置以及顶点位置发送给像素着色器
    // 这样我们就不用在像素着色器里进行矩阵乘法了
    // 这是一个极佳的优化, 因为顶点着色器通常比像素着色器运行的少
    // 这也是为什么这种方法是一种更好的实现方式的原因
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    gl_Position = model * vec4(aPos, 1.0);
    vs_out.TangentFragPos = TBN * vec3(gl_Position);
    gl_Position = projection * view *gl_Position;
}