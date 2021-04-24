#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec3 fColor;  // 颜色
out vec3 wPos;    // 世界坐标系位置
out vec3 wNormal; // 世界坐标系法向

void main() {
    fColor = aColor;
    wPos = vec3(model * vec4(aPos, 1.0f)); // 第 4 维一定为 1.0
    wNormal = mat3(transpose(inverse(model))) * aNormal;
    
    // 以光源为视点的观察坐标系
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0f);
}