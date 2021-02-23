#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 matrixForNormal; // 法线变换矩阵

void main() {
    // 因为没有投影, 所以依然是标准齐次坐标
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    gl_Position = projection * vec4(fragPos, 1.0);
    TexCoords = aTexCoords;
    normal = normalize(vec3(matrixForNormal * vec4(aNormal, 1.0)));
}