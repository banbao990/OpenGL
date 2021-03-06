#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(hdr) {
        // Reinhard 色调映射
        // 我们不再会在场景明亮的地方损失细节
        // 当然, 这个算法是倾向明亮的区域的
        // 暗的区域会不那么精细也不那么有区分度
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));
        // exposure 指数色调映射
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    } else {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}