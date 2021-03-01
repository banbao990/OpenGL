#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 Normal;
} vs_out;

void main() {
    // 几何着色器接受的位置向量是剪裁空间坐标
    // 所以我们应该将法向量变换到相同的空间中
    // 由于投影变换矩阵中的第3维已经是深度信息(而不是z), 法线的变换不能够简单的(PVM)
    // 我的想法是在观察坐标系中计算出原始的点和法向量(此时正确)
    // 然后此时计算出法向量的两个顶点, 再使用投影变换即可
    
    // 不能在模型坐标系/世界坐标系中进行
    // 可以参考法向变换矩阵推导过程
    
    // 把计算放到 geometry shader 里面
    gl_Position = vec4(aPos, 1.0);
    vs_out.Normal = normalize(aNormal);
}