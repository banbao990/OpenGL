#version 460 core
out vec4 FragColor;

// ��Ļ���Ϊ��ɫ, �Ҳ�Ϊ��ɫ
void main() {
    if(gl_FragCoord.x < 400)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green
}