#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 matrixForNormal; 
// G = (M^(-1))^T 
// G = transpose(inverse(M))

out vec3 normal;
// 观察坐标系中的位置
out vec3 fragPos;

void main() {
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    normal = vec3(matrixForNormal * vec4(aNormal, 1.0));
    gl_Position = projection * vec4(fragPos, 1.0);
}