#version 460 core

// 指定渲染到具体哪一个颜色缓冲
layout (location = 0) out vec3 gPosition; // 位置缓冲
layout (location = 1) out vec3 gNormal;   // 法向缓冲
layout (location = 2) out vec3 gFlux;     // 通量缓冲

in vec3 fColor;  // 颜色
in vec3 wPos;    // 世界坐标系位置
in vec3 wNormal; // 世界坐标系法向

uniform vec3 lightPos;

void main() {
    // 保存世界坐标系位置
    gPosition = wPos;
    
    // 保存世界坐标系法向
    gNormal = wNormal;
    
    // 计算通量并保存
    // 漫反射分量(Phong)
    // 认为光源是白色
    vec3 lightDir = normalize(lightPos - wPos);
    float diff = max(dot(lightDir, gNormal), 0.0);
    gFlux = vec3(diff * fColor);
}