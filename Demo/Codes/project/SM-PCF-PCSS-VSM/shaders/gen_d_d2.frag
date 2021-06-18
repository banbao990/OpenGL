#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D d_d2;
uniform bool vertical;

#define R 5
#define R21 11

void main() {
    vec2 d = vec2(0, 0);
    vec2 texelSize = 1.0 / textureSize(d_d2, 0);
    if(vertical) {
        float r = texelSize.y;
        for(int i = -R; i <= R; ++i) {
            d += texture(d_d2, vec2(TexCoords.x, TexCoords.y + i*r)).rg;
        }
    } else {
        float r = texelSize.x;
        for(int i = -R; i <= R; ++i) {
            d += texture(d_d2, vec2(TexCoords.x + i*r, TexCoords.y)).rg;
        }
    }
    FragColor.rg = d/R21;
    // FragColor = vec4(texture(d_d2, TexCoords).rgb, 1.0);
}