# OpenGL
+ Learning OpenGL
+ ***TODO* 为尚未解决的问题**



## 1. 配置环境

+ https://github.com/banbao990/Use



## 2. 问题

### (0) 运行报错

+ 可能是 OpenGL 中版本问题
+ `main()` 函数中版本修改为 3.3

```c
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
```

```c
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
```



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
        + 没有使用到 `glad.c`
        + 用的库也不一样



### (3) 默认工作方式

+ 默认的 `lib/include/dll`
+ 加入 `glad.c` 文件



## 3. TODO

### (1) 链接报警

```bug
LINK : warning LNK4098: 默认库“MSVCRT”与其他库的使用冲突；请使用 /NODEFAULTLIB:library
```

### (2)  未编译 *Boost*



## 4. Assimp 库

+ `Open Asset Import Library`
+ 用于模型的导入
    + *Assimp* 把不同的模型文件都转换为一个统一的数据结构
+ 模型制作
    + https://www.blender.org/
    + https://www.autodesk.nl/products/3ds-max/overview
    + https://www.autodesk.com/products/maya/overview



### 4.1 下载源代码

+ 官网下载源代码
    + https://github.com/assimp/assimp/releases/
+ 此处环境
    + https://github.com/assimp/assimp/releases/tag/v5.0.1
    + *win10*
    + *VS2017*
    + *Win10* 自带的 *DirectX.12*



### 4.2 下载依赖库

#### (1) *DirectX*

+ *win10* 自带 *DirectX.12*
+ 官网下载
    + https://www.microsoft.com/en-us/download/details.aspx?id=6812



#### (2) *Boost*

+  *assimp* 是依赖 *boost* 库的，如果没有 *boost* 库只能编译出一个功能受限的版本
+ 官网下载 
    + https://dl.bintray.com/boostorg/release/1.75.0/source/
    + 这里下载的是 *boost_1_75_0.7z*
+  *assimp* 没有用到 *boost* 需要编译的部分，所以 *boost* 只需要下载解压就可以了 

```c++
// TODO
```



### 4.3 编译源代码

#### (1) 可能需要先将 *MSVC* 加入到环境路径中

#### (2) 开始编译

+ *cmake-gui*
    + 文件夹即为 *github* 下载的源代码
    + *Add-Entry*
        + *BOOST_ROOT*
            + *STRING*
            + 解压路径
    + *ASSIMP_BUILD_ZLIB*
        + 勾选 *VALUE*
        + 否则编译的时候会爆链接错误，会调用 *anaconda* 里面的库 *z.lib* 
    + *Configuration*
        + *VS2017*
        + *Win32*
+ *Generate*
    + 生成一个 *VS* 工程
+ 点开 *Assimp.sln* 使用 *VS* 进行编译
    + **生成** - **生成解决方案**
    + 结果如下

```txt
1>------ 已跳过生成: 项目: UpdateAssimpLibsDebugSymbolsAndDLLs, 配置: Debug Win32 ------
1>没有为此解决方案配置选中要生成的项目 
2>------ 已跳过生成: 项目: uninstall, 配置: Debug Win32 ------
2>没有为此解决方案配置选中要生成的项目 
3>------ 已跳过生成: 项目: INSTALL, 配置: Debug Win32 ------
3>没有为此解决方案配置选中要生成的项目 
========== 生成: 成功 0 个，失败 0 个，最新 8 个，跳过 3 个 ==========
```

+ 现在生成的库文件就在 *Code/Debug* 文件夹下
    + *assimp-vc141-mtd.lib*
    + *assimp-vc141-mtd.dll*



### 4.4 使用 *Assimp* 库

+ 配置环境

    + *assimp-vc141-mtd.dll* 加入文件夹 *dll*

    + *include* 加入文件夹 *include*

    + *assimp-vc141-mtd.lib* 加入文件夹 *lib*

        + **项目属性** -> **链接器** -> **输入** -> **附加依赖项** -> *assimp-vc141-mtd.lib*
            + 或者代码中添加

            ```c++
            #pragma comment (lib, "assimp-vc141-mtd.lib")
            ```

+ 测试代码

    + [模型](http://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz)

```c++
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>

#pragma comment (lib, "assimp-vc141-mtd.lib")

void LoadFinish(const aiScene* scene) {
    std::cout << "LoadFinish ! NumVertices : " << (*(scene->mMeshes))->mNumVertices << std::endl;
}

bool LoadModel(const std::string& pFile) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(pFile,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    // If the import failed, report it
    if (!scene) {
        std::cout << importer.GetErrorString() << std::endl;
        return false;
    }

    // Now we can access the file's contents.
    LoadFinish(scene);

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

int main() {
    LoadModel("bunny/reconstruction/bun_zipper.ply");
    return 0;
}
```

+ 输出

```txt
LoadFinish ! NumVertices : 34834
```

 

### 4.5 参考资料

+ https://blog.csdn.net/u011371324/article/details/77142121
+ https://my.oschina.net/u/4390260/blog/4256715
+ https://blog.csdn.net/zhanxi1992/article/details/107804221