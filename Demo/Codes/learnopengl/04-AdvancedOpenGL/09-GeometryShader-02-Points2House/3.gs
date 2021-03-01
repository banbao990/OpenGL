#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;
// 三角形带, 类似于之前的线段, 复用之前的两个顶点
// 绘制 N 个三角形 需要 N+2 个顶点

// 因为几何着色器是作用于输入的一组顶点的
// 从顶点着色器发来输入数据总是会以数组的形式表示出来
// 即便我们现在只有一个顶点

in VS_OUT {
    vec3 color;
} gs_in[];


// 因为片段着色器只需要一个(插值的)颜色, 发送多个颜色并没有什么意义
// 所以, fColor向量就不是一个数组, 而是一个单独的向量
// 当发射一个顶点的时候, 每个顶点将会使用最后在fColor中储存的值, 来用于片段着色器的运行
// 对我们的房子来说, 我们只需要在第一个顶点发射之前, 使用顶点着色器中的颜色填充 fColor 一次就可以了
out vec3 fColor; // 传递给 fragment shader 的颜色信息

void build_house(vec4 position) {
    fColor = gs_in[0].color;
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
    EmitVertex();
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
    EmitVertex();
    EndPrimitive();
}

void main() {
    build_house(gl_in[0].gl_Position);
}