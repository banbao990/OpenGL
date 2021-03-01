#version 460 core
out vec4 FragColor;

// 屏幕左侧为红色, 右侧为绿色
void main() {
    if(gl_FragCoord.x < 400)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green
}