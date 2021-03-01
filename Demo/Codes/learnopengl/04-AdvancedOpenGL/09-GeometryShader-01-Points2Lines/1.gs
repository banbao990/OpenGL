#version 460 core

// �����Ӷ�����ɫ�������ͼԪ����
// �����ڵ����ֱ�ʾ����һ��ͼԪ����������С������
// points               ���� GL_POINTS ͼԪʱ (1)
// lines                ���� GL_LINES �� GL_LINE_STRIPʱ (2)
// lines_adjacency      GL_LINES_ADJACENCY �� GL_LINE_STRIP_ADJACENCY (4)
// triangles            GL_TRIANGLES, GL_TRIANGLE_STRIP �� GL_TRIANGLE_FAN (3)
// triangles_adjacency  GL_TRIANGLES_ADJACENCY �� GL_TRIANGLE_STRIP_ADJACENCY (6)

// �����ǵ�
layout (points) in;


// ����Ҫָ��������ɫ�������ͼԪ����
// points, line_strip, triangle_strip

// ���ｫ��ת��Ϊ��, ���ҽ���󶥵�������Ϊ 2 ��
// Line Strip(����), ������ֱ�����Ӹ�����һ���, �γ�����
layout (line_strip, max_vertices = 2) out;

void main() {
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    // ÿ�ε��� EmitVertex() ʱ, gl_Position �е������ᱻ��ӵ�ͼԪ����
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    // ������ EndPrimitive(), ������������ϳ�Ϊһ�������������������
    EndPrimitive();
}