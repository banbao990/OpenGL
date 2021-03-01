#version 460 core

// gl_FragCoord.z 保存着深度信息
// 投影矩阵逆变换
// 投影矩阵的工作
// http://www.songho.ca/opengl/gl_projectionmatrix.html
// https://banbao991.github.io/2021/01/31/CG/LS/05-1/

out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth)  {
    // back to NDC [0, 1] => [-1, 1]
    float z = depth * 2.0 - 1.0; 
    // 投影矩阵逆变换 
    return (2.0 * near * far) / (far + near - z * (far - near)); 
}

void main() {             
    // 为了演示, 线性归一化到 [0, 1] 区间内
    float depth = LinearizeDepth(gl_FragCoord.z);
    depth = (depth - near) / (far - near); 
    FragColor = vec4(vec3(depth), 1.0);
}