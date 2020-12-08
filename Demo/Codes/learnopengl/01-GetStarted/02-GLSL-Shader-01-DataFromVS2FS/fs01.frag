#version 460 core

// 片段着色器的输入输出
out vec4 FragColor;
// 从顶点着色器传来的输入变量(名称相同 && 类型相同)
in vec4 vertexColor; 

void main() {
    FragColor = vertexColor;
}