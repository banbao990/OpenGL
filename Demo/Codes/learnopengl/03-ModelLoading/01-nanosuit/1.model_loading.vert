#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 matrixForNormal; // ���߱任����

void main() {
    // ��Ϊû��ͶӰ, ������Ȼ�Ǳ�׼�������
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    gl_Position = projection * vec4(fragPos, 1.0);
    TexCoords = aTexCoords;
    normal = normalize(vec3(matrixForNormal * vec4(aNormal, 1.0)));
}