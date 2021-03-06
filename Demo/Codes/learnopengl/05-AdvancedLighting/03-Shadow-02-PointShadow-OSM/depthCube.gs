#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main() {
    // �л����Թ�ԴΪ�ӵ�Ĺ۲�����ϵ��
    // 6 ����ͬ��ͶӰ����
    for(int face = 0; face < 6; ++face) {
        // gl_Layer ָ����ɢ������ͼ���͵���������ͼ���ĸ���
        // ��������ʱ, ������ɫ���ͻ�������һ�������Ļ���ͼ�η��͵����͹ܵ�����һ�׶�
        // �������Ǹ�������������ܿ���ÿ������ͼ�ν���Ⱦ����������ͼ����һ����
        // ��Ȼ��ֻ�е���������һ�����ӵ������֡�������������ͼ�������Ч
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