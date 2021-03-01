#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

// 由于没有内置的接口块, 需要自己定义

// 接口块在几何着色器这样的着色器中会更容易处理一点
// 实际上, 几何着色器的输入能够变得非常大
// 将它们合并为一个大的接口块数组会更符合逻辑一点

out VS_OUT {
    vec3 color;
} vs_out;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.color = aColor;
}