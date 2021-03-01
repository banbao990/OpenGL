#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in vec3 vs_out_color[]; // ʹ�� vec3 ����(��ʹ�ýӿڿ�)
out vec3 fColor; // ���ݸ� fragment shader ����ɫ��Ϣ

void build_house(vec4 position) {
    fColor = vs_out_color[0];
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