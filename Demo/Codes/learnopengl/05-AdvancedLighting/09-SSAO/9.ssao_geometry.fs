#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec3 FragPos;
in vec3 Normal;

// 整体是在观察坐标系中的
void main() {
    // 位置
    gPosition = FragPos;
    
    // 法向
    gNormal = normalize(Normal);
    
    // 漫反射
    gAlbedo.rgb = vec3(0.95);
}