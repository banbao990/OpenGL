#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// declare an interface block
out VS_OUT {
    vec3 FragPos; // 世界坐标系位置
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool reverse_normals; // 渲染整个 cube 环境的时候, 从里面渲染(光照正常)


void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // 世界坐标系位置
    if(reverse_normals) {
        vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
    } else {
        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    }
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}