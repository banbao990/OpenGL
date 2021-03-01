# README

+ 纹理文件 skybox(6个纹理文件) 在如下文件夹中
    + 04-AdvancedOpenGL/06-CubeMaps-01-Skybox
+ 模型文件 nanosiot_reflection 在如下文件夹中
    + 04-AdvancedOpenGL/06-CubeMaps-02-Nanosuit
    
+ 一些疑惑
    + assimp 中是支持 reflection maps 的, 不知道为什么 model.h 没用
        + material.h
    + nanosuit 中把反射贴图保存为了 ambient
        + nanosuit.mtl

+ 能够看到一些反射贴图的效果, 身上有山水, 眼睛没有
+ 图片水平翻转, 可以在 shader 里面采样的时候 .x = -.x 处理
