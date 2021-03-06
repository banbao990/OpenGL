#version 460 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

// 需要自己计算深度信息
void main() {
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    // 归一化
    // lightDistance = (lightDistance - near_plane) / (far_plane - near_plane);
    // 注意光源本质上是没有做裁剪的
    lightDistance = lightDistance / far_plane;
    // 深度写入
    gl_FragDepth = lightDistance;
}