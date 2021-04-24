#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform bool isDepth;
uniform float nearPlane;
uniform float farPlane;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main() {
    if(isDepth) {
        float depthValue = texture(scene, TexCoords).r;
        FragColor = vec4(vec3(LinearizeDepth(depthValue) / farPlane), 1.0); 
    } else {
        FragColor = vec4(texture(scene, TexCoords).rgb, 1.0f);
    }
}