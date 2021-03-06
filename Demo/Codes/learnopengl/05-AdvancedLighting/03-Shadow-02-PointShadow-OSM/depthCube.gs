#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main() {
    // 切换到以光源为视点的观察坐标系中
    // 6 个不同的投影方向
    for(int face = 0; face < 6; ++face) {
        // gl_Layer 指定发散出基本图形送到立方体贴图的哪个面
        // 当不管它时, 几何着色器就会像往常一样把它的基本图形发送到输送管道的下一阶段
        // 但当我们更新这个变量就能控制每个基本图形将渲染到立方体贴图的哪一个面
        // 当然这只有当我们有了一个附加到激活的帧缓冲的立方体贴图纹理才有效
        gl_Layer = face;
        for(int i = 0; i < 3; ++i) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

// gl_Layer:
// 0    GL_TEXTURE_CUBEMAP_POSITIVE_X
// 1    GL_TEXTURE_CUBEMAP_NEGATIVE_X
// 2    GL_TEXTURE_CUBEMAP_POSITIVE_Y
// 3    GL_TEXTURE_CUBEMAP_NEGATIVE_Y
// 4    GL_TEXTURE_CUBEMAP_POSITIVE_Z
// 5    GL_TEXTURE_CUBEMAP_NEGATIVE_Z