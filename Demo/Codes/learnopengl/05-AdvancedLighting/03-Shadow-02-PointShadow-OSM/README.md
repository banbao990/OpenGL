# README

+ 纹理文件 wood.png 在如下文件夹中
    + 05-AdvancedLighting/01-BlinnPhong

```shell
# 之前的 shadow mapping
# 光源动起来, 地面上的阴影时有时无(视锥体不能覆盖的原因)
Demo1.exe

# 阴影立方体贴图
Demo2.exe
# 想要显示阴影贴图的话可以修改 farAllSeeCube.fs
# FragColor = vec4(vec3(closestDepth / far_plane), 1.0);

# PCF
Demo2.exe farAllSeeCubePCF.vs farAllSeeCubePCF.fs
Demo2.exe farAllSeeCubePCF2.vs farAllSeeCubePCF2.fs
Demo2.exe farAllSeeCubePCF3.vs farAllSeeCubePCF3.fs
```