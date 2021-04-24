#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec3 fColor;      // 颜色
out vec3 fPosLight;   // 以光源为视点的坐标系中的位置
out vec3 fPos;        // 世界坐标系中的位置
out vec3 fNormal;     // 世界坐标系中的法向

void main() {
    fColor = aColor;
    vec4 tPos = lightSpaceMatrix * vec4(aPos, 1.0f);
    fPosLight = tPos.xyz / tPos.w;
    fPos = vec3(model * vec4(aPos, 1.0f));
    fNormal = mat3(transpose(inverse(model))) * aNormal;
    // 最终的位置
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}