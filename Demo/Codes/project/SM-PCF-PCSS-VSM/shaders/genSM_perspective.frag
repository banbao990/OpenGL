#version 330 core
out vec4 FragColor;

#define NEAR_PLANE 2.0
#define FAR_PLANE 20.0

// ����Ϊ [0, 1] ��ͶӰ���
// ���Ϊ [0, 1] ���������
float getLinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC
    // ��ͨ������ [-1, 1] ��֤���Ϊ [NEAR_PLANE, FAR_PLANE]
    z =(2.0 * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));
    return (z - NEAR_PLANE)/(FAR_PLANE - NEAR_PLANE);
}


void main() {
    // pass
    // gl_FragDepth = gl_FragCoord.z;

    // �����ʾ, ���� DEBUG
    // gl_FragColor = vec4(vec3(gl_FragCoord.z<0?0.0:1.0), 1.0);
    float depth = getLinearizeDepth(gl_FragCoord.z);
    FragColor.r = depth;
    FragColor.g = depth * depth;
}