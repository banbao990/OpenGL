#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// declare an interface block
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace; // 以光源为视点的观察坐标系中点的位置
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix; // 变换矩阵


void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // 世界坐标系位置
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal; // 法向变换矩阵
    vs_out.TexCoords = aTexCoords;
    // 以光源为视点的观察坐标系中点的位置
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}