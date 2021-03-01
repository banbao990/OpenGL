#version 460 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in VS_OUT {
    vec3 Normal;// 已经归一化
} gs_in[];

const float MAGNITUDE = 0.1;

void GenerateLine(int index, mat4 VM, mat3 normal_VM) {
    
    vec4 pos_VM = VM * gl_in[index].gl_Position;
    
    // 法线第一个顶点
    vec4 pos1 = projection * pos_VM;
    gl_Position = pos1;
    EmitVertex();
    
    // 法线第二个顶点
    /* 可能由于投影的原因导致法线很长
    gl_Position = projection * (
        pos_VM + vec4(
            MAGNITUDE * normalize(normal_VM * gs_in[index].Normal),
            0.0
        )
    );
    */
    gl_Position = projection * (
        pos_VM + vec4(
            normal_VM * gs_in[index].Normal,
            0.0
        )
    );
    // 控制法向量的长短
    gl_Position = MAGNITUDE*normalize(gl_Position - pos1) + pos1;
    EmitVertex();
    EndPrimitive(); // 注意每条发现绘制结束之后都需要调用 EndPrmitive()
}

void main() {
    mat4 VM = view * model;
    mat3 normal_VM = mat3(transpose(inverse(VM))); // 法线变换矩阵
    
    GenerateLine(0, VM, normal_VM); // 第一个顶点法线
    GenerateLine(1, VM, normal_VM); // 第二个顶点法线
    GenerateLine(2, VM, normal_VM); // 第三个顶点法线
}
