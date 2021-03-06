# README

```shell
# 不使用 TBN, 法线有问题()
Demo1.exe

# TBN
# 看区别的一个方法, brick 旋转 180 度, 旋转光源看变化
Demo2.exe TBN.vs TBN.fs
Demo2.exe TBN2.vs TBN2.fs
```



## TBN 推导

+ 利用坐标和纹理坐标计算出 tangent，bitangent

![](img/normal_mapping_surface_edges.png)



+ 列出方程

$$
E1=ΔU1T+ΔV1B\\
E2=ΔU2T+ΔV2B
$$

+ 维度展开

$$
(E1x,E1y,E1z)=ΔU1(Tx,Ty,Tz)+ΔV1(Bx,By,Bz)\\
(E2x,E2y,E2z)=ΔU2(Tx,Ty,Tz)+ΔV2(Bx,By,Bz)
$$

+ 矩阵形式

$$
\left[
\begin{array}{l}
E_{1x}&E_{1y}&E_{1z}\\
E_{2x}&E_{2y}&E_{2z}
\end{array}
\right]
=
\left[
\begin{array}{l}
\Delta U_{1}&\Delta V_{1}\\
\Delta U_{2}&\Delta V_{2}
\end{array}
\right]
\cdot
\left[
\begin{array}{l}
T_{x}&T_{y}&T_{z}\\
B_{x}&B_{y}&B_{z}
\end{array}
\right]
$$

+ 求解即可

$$
\left[
\begin{array}{l}
T_{x}&T_{y}&T_{z}\\
B_{x}&B_{y}&B_{z}
\end{array}
\right]
=
\left[
\begin{array}{l}
\Delta U_{1}&\Delta V_{1}\\
\Delta U_{2}&\Delta V_{2}
\end{array}
\right]^{-1}
\cdot
\left[
\begin{array}{l}
E_{1x}&E_{1y}&E_{1z}\\
E_{2x}&E_{2y}&E_{2z}
\end{array}
\right]
$$

$$
\left[
\begin{array}{l}
T_{x}&T_{y}&T_{z}\\
B_{x}&B_{y}&B_{z}
\end{array}
\right]
=
\dfrac{1}{\Delta U_{1}\Delta V_{2}-\Delta V_{2}\Delta U_{1}}
\left[
\begin{array}{l}
\Delta V_{2}&-\Delta V_{1}\\
\Delta -U_{2}&\Delta U_{2}
\end{array}
\right]
\cdot
\left[
\begin{array}{l}
E_{1x}&E_{1y}&E_{1z}\\
E_{2x}&E_{2y}&E_{2z}
\end{array}
\right]
$$



+ 归一化之后再叉乘求出 normal



## 复杂物体的法线加载

```cpp
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    // 一个技巧
    // 可以在不必花费太多性能开销的情况下稍稍提升画质表现
    // 当在更大的网格上计算切线向量的时候, 它们往往有很大数量的共享顶点
    // 当法向贴图应用到这些表面时将切线向量平均化通常能获得更好更平滑的结果
    // 这样做有个问题, 就是TBN向量可能会不能互相垂直
    // 这意味着TBN矩阵不再是正交矩阵了
    // 法线贴图可能会稍稍偏移，但这仍然可以改进
    // 使用叫做格拉姆-施密特正交化过程(Gram-Schmidt process)的数学技巧
    // 我们可以对TBN向量进行重正交化, 这样每个向量就又会重新垂直了
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

