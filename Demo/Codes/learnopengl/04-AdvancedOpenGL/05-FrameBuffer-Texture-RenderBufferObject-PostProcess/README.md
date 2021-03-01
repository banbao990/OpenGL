# README

+ 纹理文件 metal.png 在如下文件夹中
    + 04-AdvancedOpenGL/01-DepthTesting-Function-Visualization/
+ 纹理文件 container.jpg 在如下文件夹中
    + 01-GetStarted/03-Textures-02-MoreTextures



## post-process 后期处理

+ 通过修改 5.1.framebuffers_screen.vs/fs 进行后处理
+ 类似于图像处理了
+ 直接通过命令行输入参数即可


```shell
# 原始图像 origin
Demo.exe origin.vs origin.fs

# 反相  Inversion
Demo.exe inversion.vs inversion.fs

# 灰度 grayscale
Demo.exe grayscale.vs grayscale.fs

# 灰度加权 weighted grayscale
# 人眼会对绿色更加敏感一些, 而对蓝色不那么敏感
Demo.exe weightedGrayscale.vs weightedGrayscale.fs

# 核函数(卷积)
# 锐化(拉普拉斯核) Laplacian
Demo.exe lapacian.vs lapacian.fs

# 模糊 blur
Demo.exe blur.vs blur.fs

# 边缘检测(拉普拉斯核) Laplacian
Demo.exe lapacianEdge.vs lapacianEdge.fs
```