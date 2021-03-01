#version 460 core

// gl_FragCoord.z �����������Ϣ
// ͶӰ������任
// ͶӰ����Ĺ���
// http://www.songho.ca/opengl/gl_projectionmatrix.html
// https://banbao991.github.io/2021/01/31/CG/LS/05-1/

out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth)  {
    // back to NDC [0, 1] => [-1, 1]
    float z = depth * 2.0 - 1.0; 
    // ͶӰ������任 
    return (2.0 * near * far) / (far + near - z * (far - near)); 
}

void main() {             
    // Ϊ����ʾ, ���Թ�һ���� [0, 1] ������
    float depth = LinearizeDepth(gl_FragCoord.z);
    depth = (depth - near) / (far - near); 
    FragColor = vec4(vec3(depth), 1.0);
}