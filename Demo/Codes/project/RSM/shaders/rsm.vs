#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec3 fColor;      // ��ɫ
out vec3 fPosLight;   // �Թ�ԴΪ�ӵ������ϵ�е�λ��
out vec3 fPos;        // ��������ϵ�е�λ��
out vec3 fNormal;     // ��������ϵ�еķ���

void main() {
    fColor = aColor;
    vec4 tPos = lightSpaceMatrix * vec4(aPos, 1.0f);
    fPosLight = tPos.xyz / tPos.w;
    fPos = vec3(model * vec4(aPos, 1.0f));
    fNormal = mat3(transpose(inverse(model))) * aNormal;
    // ���յ�λ��
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}