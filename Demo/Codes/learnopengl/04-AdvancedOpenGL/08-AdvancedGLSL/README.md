# README

+ https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL)

## 一些 GLSL 变量

### 顶点着色器 vertex shader

+ **gl_Position**

    + 顶点着色器的裁剪空间输出位置向量

+ **gl_PointSize**

    + 需要在 **GL_POINTS** 绘制模式下才有效果
    + 点的大小
    + the [pixel width/height of the point](https://www.khronos.org/opengl/wiki/Point_Sprite) being rasterized. It only has a meaning when rendering point primitives, which in a TES requires using the point_mode input layout qualifier. 

    ```c++
    // 开启后在能在 vertex shader 中修改点的大小
    glEnable(GL_PROGRAM_POINT_SIZE);
    ```

+ **gl_VertexID**
    + gl_Position和gl_PointSize都是**输出变量**，因为它们的值是作为顶点着色器的输出被读取的。我们可以对它们进行写入，来改变结果
    + 顶点着色器还为我们提供了一个有趣的**输入变量**，我们**只能对它进行读取**，它叫做gl_VertexID
    + 整型变量gl_VertexID储存了正在绘制顶点的当前ID
    + 当（使用glDrawElements）进行索引渲染的时候，这个变量会存储正在**绘制顶点的当前索引**
    + 当（使用glDrawArrays）不使用索引进行绘制的时候，这个变量会储存**从渲染调用开始的已处理顶点数量**



### fragment shader 片段着色器

+  GLSL提供给我们两个有趣的输入变量：gl_FragCoord和gl_FrontFacing。 

+ **gl_FragCoord**
    + gl_FragCoord的z分量等于对应片段的深度值
    + gl_FragCoord的x和y分量是片段的窗口空间(Window-space)坐标，其**原点为窗口的左下角**
    + 我们已经使用 glViewport 设定了一个800x600的窗口了，所以片段窗口空间坐标的x分量将在0到800之间，y分量在0到600之间 
+ **gl_FrontFacing**
    + bool 变量
    + 如果我们不（启用GL_FACE_CULL来）使用面剔除，那么gl_FrontFacing将会告诉我们当前片段是属于正向面的一部分还是背向面的一部分 
        + 如果当前片段是正向面的一部分那么就是`true`，否则就是`false`
    + 一个例子：**在立方体内部和外部使用不同的纹理**
    + 注意，如果你开启了面剔除，你就看不到箱子内部的面了，所以现在再使用gl_FrontFacing就没有意义了 

+ **gl_FragDepth**

    + 输入变量gl_FragCoord能让我们读取当前片段的窗口空间坐标，并获取它的深度值，但是它是一个**只读变量**，我们不能修改片段的窗口空间坐标，但实际上修改片段的深度值还是可能的
    + GLSL提供给我们一个叫做gl_FragDepth的输出变量，我们可以使用它来在着色器内**设置片段的深度值**
    +  **缺点**
        + 只要我们在片段着色器中对gl_FragDepth进行写入，OpenGL就会禁用所有的提前深度测试
        + 它被禁用的原因是，OpenGL无法在片段着色器运行**之前**得知片段将拥有的深度值，因为片段着色器可能会完全修改这个深度值 
    +  在写入gl_FragDepth时，你就需要考虑到它所带来的性能影响
    + 然而，从OpenGL 4.2起，我们仍可以对两者进行一定的调和，在片段着色器的顶部使用深度条件(Depth Condition)重新声明gl_FragDepth变量

    ```c++
    layout (depth_<condition>) out float gl_FragDepth;
    ```

    |    条件     |                             描述                             |
    | :---------: | :----------------------------------------------------------: |
    |    `any`    |        默认值。提前深度测试是禁用的，你会损失很多性能        |
    |  `greater`  |             你只能让深度值比`gl_FragCoord.z`更大             |
    |   `less`    |             你只能让深度值比`gl_FragCoord.z`更小             |
    | `unchanged` | 如果你要写入`gl_FragDepth`，你将只能写入`gl_FragCoord.z`的值 |



### 接口块

+ 到目前为止，每当我们希望从顶点着色器向片段着色器发送数据时，我们都声明了几个对应的输入/输出变量。将它们一个一个声明是着色器间发送数据最简单的方式了，但当程序变得更大时，你希望发送的可能就不只是几个变量了，它还可能包括数组和结构体。
+ 为了帮助我们管理这些变量，GLSL为我们提供了一个叫做接口块(Interface Block)的东西，来方便我们组合这些变量。接口块的声明和struct的声明有点相像，不同的是，现在根据它是一个输入还是输出块(Block)，使用in或out关键字来定义的
+ 一个例子

```c++
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec2 TexCoords;
} vs_out;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);    
    vs_out.TexCoords = aTexCoords;
}  
```

```c++
#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture;

void main() {             
    FragColor = texture(texture, fs_in.TexCoords);   
}
```



### Uniform 缓冲对象

+ 当使用多于一个的着色器时，尽管大部分的uniform变量都是相同的，我们还是需要不断地设置它们，所以为什么要这么麻烦地重复设置它们呢？

+ OpenGL为我们提供了一个叫做Uniform缓冲对象(Uniform Buffer Object)的工具，它允许我们定义一系列在多个着色器中相同的**全局**Uniform变量。当使用Uniform缓冲对象的时候，我们只需要设置相关的uniform**一次**。当然，我们仍需要手动设置每个着色器中不同的uniform。并且创建和配置Uniform缓冲对象会有一点繁琐。

+ 因为Uniform缓冲对象仍是一个缓冲，我们可以使用glGenBuffers来创建它，将它绑定到GL_UNIFORM_BUFFER缓冲目标，并将所有相关的uniform数据存入缓冲。在Uniform缓冲对象中储存数据是有一些规则的，我们将会在之后讨论它。首先，我们将使用一个简单的顶点着色器，将projection和view矩阵存储到所谓的Uniform块(Uniform Block)中：

```c++
#version 460 core
layout (location = 0) in vec3 aPos;

# uniform 缓冲块
layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

+  `layout (std140)`
    + 是指当前定义的Uniform块对它的内容使用一个特定的内存布局
    + 这个语句设置了Uniform块布局(Uniform Block Layout)



#### Uniform 块布局

+ 默认情况下，GLSL会使用一个叫做共享(Shared)布局的Uniform内存布局，共享是因为一旦硬件定义了偏移量，它们在多个程序中是**共享**并一致的 
+ 虽然共享布局给了我们很多节省空间的优化，但是我们需要查询每个uniform变量的偏移量，这会产生非常多的工作量。通常的做法是，不使用共享布局，而是使用**std140布局**
+ std140布局声明了每个变量的偏移量都是由一系列规则所决定的，这**显式地**声明了每个变量类型的内存布局。由于这是显式提及的，我们可以手动计算出每个变量的偏移量。
+ 每个变量都有一个基准对齐量(Base Alignment)，它等于一个变量在Uniform块中所占据的空间（包括填充量(Padding)），这个基准对齐量是使用std140布局的规则计算出来的。接下来，对每个变量，我们再计算它的对齐偏移量(Aligned Offset)，它是一个变量从块起始位置的字节偏移量。一个变量的对齐字节偏移量**必须**等于基准对齐量的倍数。 
+ 布局规则的原文可以在OpenGL的Uniform缓冲规范[这里](http://www.opengl.org/registry/specs/ARB/uniform_buffer_object.txt)找到，但我们将会在下面列出最常见的规则。GLSL中的每个变量，比如说int、float和bool，都被定义为4字节量。每4个字节将会用一个`N`来表示

|          类型          |                           布局规则                           |
| :--------------------: | :----------------------------------------------------------: |
| 标量，比如 int 和 bool |                  每个标量的基准对齐量为N。                   |
|          向量          |           2N或者4N。这意味着vec3的基准对齐量为4N。           |
|    标量或向量的数组    |           **每个元素**的基准对齐量与**vec4**的相同           |
|          矩阵          |    储存为列向量的数组，每个向量的基准对齐量与vec4的相同。    |
|         结构体         | 等于所有元素根据规则计算后的大小，但会填充到vec4大小的倍数。 |

+ 一个例子

```c++
layout (std140) uniform ExampleBlock {
                     // 基准对齐量       // 对齐偏移量
    float value;     // 4               // 0 
    vec3 vector;     // 16              // 16  (必须是16的倍数，所以 4->16)
    mat4 matrix;     // 16              // 32  (列 0)
                     // 16              // 48  (列 1)
                     // 16              // 64  (列 2)
                     // 16              // 80  (列 3)
    float values[3]; // 16              // 96  (values[0])
                     // 16              // 112 (values[1])
                     // 16              // 128 (values[2])
    bool boolean;    // 4               // 144
    int integer;     // 4               // 148
}; 
```



#### 使用Uniform缓冲

 

![](img/advanced_glsl_binding_points.png)

+ 首先，我们需要调用 glGenBuffers，创建一个 Uniform 缓冲对象。一旦我们有了一个缓冲对象，我们需要将它绑定到 GL_UNIFORM_BUFFER 目标，并调用 glBufferData，分配足够的内存。 

```c++
// 生成 UBO
unsigned int uboExampleBlock;
glGenBuffers(1, &uboExampleBlock);
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW); // 分配152字节的内存
glBindBuffer(GL_UNIFORM_BUFFER, 0);

// shader 绑定, 注意我们需要对每个着色器重复这一步骤
// ID, uniform buffer 名称
unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, "Lights");
// 我们可以用以下方式将图示中的 Lights Uniform 块链接到绑定点 2
glUniformBlockBinding(shaderA.ID, lights_index, 2);

// 从OpenGL 4.2版本起, 你也可以添加一个布局标识符, 显式地将Uniform块的绑定点储存在着色器中
// 这样就不用再调用 glGetUniformBlockIndex 和 glUniformBlockBinding 了
// 下面的代码显式地设置了 Lights Uniform 块的绑定点
/*
layout(std140, binding = 2) uniform Lights { ... };
*/


// 我们还需要绑定 Uniform 缓冲对象到相同的绑定点上
// 这可以使用 glBindBufferBase 或 glBindBufferRange 来完成
// 通过使用glBindBufferRange函数，你可以让多个不同的Uniform块绑定到同一个Uniform缓冲对象上
glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock); 
// 或
glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, 152);

// 添加数据(部分)
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
int b = true; // GLSL中的bool是4字节的，所以我们将它存为一个integer
glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &b); 
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```



#### 好处

+ Uniform缓冲对象比起独立的uniform有很多好处
    + 一次设置很多uniform会比一个一个设置多个uniform要快很多
    + 比起在多个着色器中修改同样的uniform，在Uniform缓冲中修改一次会更容易一些
    + 最后一个好处可能不会立即显现，如果使用Uniform缓冲对象的话，你可以在着色器中使用更多的uniform。OpenGL限制了它能够处理的uniform数量，这可以通过GL_MAX_VERTEX_UNIFORM_COMPONENTS来查询。当使用Uniform缓冲对象时，最大的数量会更高。所以，当你达到了uniform的最大数量时（比如再做骨骼动画(Skeletal Animation)的时候），你总是可以选择使用Uniform缓冲对象。 



## 代码使用

```shell
# 单纯绘制一个立方体
Demo.exe 1.vs 1.fs

# gl_PointSize
Demo.exe 2.vs 2.fs

# gl_FragCoord
Demo.exe 3.vs 3.fs

# gl_FrontFacing
Demo.exe 3.vs 3.fs

# uniform buffer
Demo2.exe

# gl_PointSize
Demo3.exe   # on
Demo3.exe x # off(很仔细看才能看到小点点)
```

