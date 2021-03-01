#version 460 core
out vec4 FragColor;

// gl_FragCoord.z 保存着深度信息
// 非线性变换, 基本全为白色
// 在z值很小的时候有很高的精度, 而z值很大的时候有较低的精度
// 如果我们小心地靠近物体, 可以看到颜色会渐渐变暗, 显示它们的z值在逐渐变小

void main() {
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}