#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in vec3 vs_out_color[]; // 使用 vec3 传递(不使用接口块)
out vec3 fColor; // 传递给 fragment shader 的颜色信息

void build_house(vec4 position) {
    fColor = vs_out_color[0];
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