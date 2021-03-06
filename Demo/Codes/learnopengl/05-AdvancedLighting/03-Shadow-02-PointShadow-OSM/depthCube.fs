#version 460 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

// ��Ҫ�Լ����������Ϣ
void main() {
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    // ��һ��
    // lightDistance = (lightDistance - near_plane) / (far_plane - near_plane);
    // ע���Դ��������û�����ü���
    lightDistance = lightDistance / far_plane;
    // ���д��
    gl_FragDepth = lightDistance;
}