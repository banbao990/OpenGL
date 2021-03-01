#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1; // 具体是哪一张纹理图

void main() {             
    vec4 texColor = texture(texture1, TexCoords);
    // 透明度小于阈值 0.1 时, 丢弃当前的 fragment
    // 注释掉下面 3 行则不丢弃
    if(texColor.a < 0.1) {
        discard;
    }
    FragColor = texColor;
}