#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {

    // 法线变换矩阵
    mat3 normalModel = mat3(transpose(inverse(model)));

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // 世界坐标系位置
    vs_out.Normal = normalModel * aNormal; // 法向变换矩阵
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec3 T   = normalize(normalModel * aTangent);
    vec3 B   = normalize(normalModel * aBitangent);
    vec3 N   = normalize(normalModel * aNormal);
    // 正交矩阵, 逆矩阵等于矩阵的转置
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;
}