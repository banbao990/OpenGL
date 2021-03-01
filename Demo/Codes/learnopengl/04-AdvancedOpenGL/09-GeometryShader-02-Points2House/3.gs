#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;
// �����δ�, ������֮ǰ���߶�, ����֮ǰ����������
// ���� N �������� ��Ҫ N+2 ������

// ��Ϊ������ɫ���������������һ�鶥���
// �Ӷ�����ɫ�����������������ǻ����������ʽ��ʾ����
// ������������ֻ��һ������

in VS_OUT {
    vec3 color;
} gs_in[];


// ��ΪƬ����ɫ��ֻ��Ҫһ��(��ֵ��)��ɫ, ���Ͷ����ɫ��û��ʲô����
// ����, fColor�����Ͳ���һ������, ����һ������������
// ������һ�������ʱ��, ÿ�����㽫��ʹ�������fColor�д����ֵ, ������Ƭ����ɫ��������
// �����ǵķ�����˵, ����ֻ��Ҫ�ڵ�һ�����㷢��֮ǰ, ʹ�ö�����ɫ���е���ɫ��� fColor һ�ξͿ�����
out vec3 fColor; // ���ݸ� fragment shader ����ɫ��Ϣ

void build_house(vec4 position) {
    fColor = gs_in[0].color;
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:����
    EmitVertex();
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:����
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:����
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:����
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:����
    EmitVertex();
    EndPrimitive();
}

void main() {
    build_house(gl_in[0].gl_Position);
}