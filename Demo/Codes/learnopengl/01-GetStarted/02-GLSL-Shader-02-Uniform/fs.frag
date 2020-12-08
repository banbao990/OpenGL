#version 460 core
out vec4 FragColor;

// uniform 是全局变量
// 可以被着色器程序的任意着色器在任意阶段访问
// 程序和着色器间数据交互

uniform vec4 ourColor;

void main() {
    FragColor = ourColor;
}