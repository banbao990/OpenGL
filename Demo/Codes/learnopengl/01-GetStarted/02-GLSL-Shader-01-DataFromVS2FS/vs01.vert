// 功能: 将输出转化为暗红色

// 着色器之间相互独立, 只能通过输入输出来通信

// 开头:版本号, 核心模式
#version 460 core

// 基本类型: vecn, bvecn, ivecn, uvecn, dvecn

// 灵活的处理方式
// vec2.x, vec2.y, vec2.xy
// vec3 = vec2.xxy + vec2.xyy

// 构造函数
// vec2(1.0, 1.0)
// vec3(vec2, 1.0)

// 输入与输出
// 顶点着色器输入/输出
// 从顶点数据中直接接收输入, 具体管理引入了 location

// location 位置变量的属性位置值为 0
layout (location = 0) in vec3 aPos;

// 为片段着色器指定一个颜色输出
out vec4 vertexColor; 
void main() {
    // 注意我们如何把一个 vec3 作为 vec4 的构造器的参数
    gl_Position = vec4(aPos, 1.0); 
    // 把输出变量设置为暗红色
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); 
}