#version 460 core

// 声明从顶点着色器输入的图元类型
// 括号内的数字表示的是一个图元所包含的最小顶点数
// points               绘制 GL_POINTS 图元时 (1)
// lines                绘制 GL_LINES 或 GL_LINE_STRIP时 (2)
// lines_adjacency      GL_LINES_ADJACENCY 或 GL_LINE_STRIP_ADJACENCY (4)
// triangles            GL_TRIANGLES, GL_TRIANGLE_STRIP 或 GL_TRIANGLE_FAN (3)
// triangles_adjacency  GL_TRIANGLES_ADJACENCY 或 GL_TRIANGLE_STRIP_ADJACENCY (6)

// 这里是点
layout (points) in;


// 还需要指定几何着色器输出的图元类型
// points, line_strip, triangle_strip

// 这里将点转化为线, 并且将最大顶点数设置为 2 个
// Line Strip(线条), 连续用直线连接给定的一组点, 形成线条
layout (line_strip, max_vertices = 2) out;

void main() {
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    // 每次调用 EmitVertex() 时, gl_Position 中的向量会被添加到图元中来
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    // 最后调用 EndPrimitive(), 将这两个顶点合成为一个包含两个顶点的线条
    EndPrimitive();
}