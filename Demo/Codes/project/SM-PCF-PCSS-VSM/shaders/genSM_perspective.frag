#version 330 core
out vec4 FragColor;

#define NEAR_PLANE 2.0
#define FAR_PLANE 20.0

// 输入为 [0, 1] 的投影深度
// 输出为 [0, 1] 的线性深度
float getLinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC
    // 简单通过代入 [-1, 1] 验证结果为 [NEAR_PLANE, FAR_PLANE]
    z =(2.0 * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));
    return (z - NEAR_PLANE)/(FAR_PLANE - NEAR_PLANE);
}


void main() {
    // pass
    // gl_FragDepth = gl_FragCoord.z;

    // 输出显示, 用于 DEBUG
    // gl_FragColor = vec4(vec3(gl_FragCoord.z<0?0.0:1.0), 1.0);
    float depth = getLinearizeDepth(gl_FragCoord.z);
    FragColor.r = depth;
    FragColor.g = depth * depth;
}