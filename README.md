# OpenGL
+ Learning OpenGL



## 1. 配置环境

+ https://github.com/banbao990/Use



## 2. 问题

### (1) 缺少配置 dll 文件

+ 将目录下的 `dll` 文件夹加入环境变量 `PATH`



### (2) VS2017 工程

+ 将目录下的 `dll` 文件夹加入环境变量 `PATH`
+ 将 `Codes` 中对应文件加入到工程的 `*.cpp` 的文件夹下
+ `lib/include/dll` 的话加入到对应的文件夹中即可
    + 为了方便直接能够运行 `*.exe` 文件，我们直接将 `*.dll` 文件放在了 `*.exe` 同目录下
    + 但是配置好环境路径之后，我们可以直接将 `*.dll` 文件加入到 `dll` 文件夹即可
+ **需要将 glad.c 加入到文件夹源码中(由于重复就没有放在 Code 文件夹下)**
    + 除了 `FromTA01`



### (3) 默认工作方式

+ 默认的 `lib/include/dll`
+ 加入 `glad.c` 文件