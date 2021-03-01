
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/shader.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/* instancing 实例化 */
// 假设你有一个绘制了很多模型的场景, 而大部分的模型包含的是同一组顶点数据
// 只不过进行的是不同的世界空间变换
// 想象一个充满草的场景: 每根草都是一个包含几个三角形的小模型, 你可能会需要绘制很多根草
// 最终在每帧中你可能会需要渲染上千或者上万根草
// 因为每一根草仅仅是由几个三角形构成, 渲染几乎是瞬间完成的
// 但上千个渲染函数调用却会极大地影响性能

// 性能产生瓶颈的原因
/*
for() {
    DoSomePreparations(); // 绑定 VAO, 绑定纹理, 设置 uniform 等
    glDrawArrays(GL_TRIANGLES, 0, amount_of_vertices);
}
*/
// 如果像这样绘制模型的大量实例(Instance), 你很快就会因为绘制调用过多而达到性能瓶颈
// 与绘制顶点本身相比, 使用 glDrawArrays 或 glDrawElements 函数告诉 GPU 去
// 绘制你的顶点数据会消耗更多的性能
// 因为 OpenGL 在绘制顶点数据之前需要做很多准备工作(比如告诉GPU该从哪个缓冲读取数据, 
// 从哪寻找顶点属性, 而且这些都是在相对缓慢的CPU到GPU总线(CPU to GPU Bus)上进行的)
// 所以, 即便渲染顶点非常快, 命令 GPU 去渲染却未必

// 如果我们能够将数据一次性发送给GPU
// 然后使用一个绘制函数让 OpenGL 利用这些数据绘制多个物体, 就会更方便了
// 这就是实例化(Instancing)

// 实例化这项技术能够让我们使用一个渲染调用来绘制多个物体
// 来节省每次绘制物体时CPU -> GPU的通信, 它只需要一次即可

// 实例化渲染
// glDrawArrays, glDrawElements 
//    => glDrawArraysInstanced, glDrawElementsInstanced
// 这些渲染函数的实例化版本需要一个额外的参数
// 叫做实例数量(Instance Count), 它能够设置我们需要渲染的实例个数

// 这个函数本身并没有什么用, 渲染同一个物体一千次对我们并没有什么用处
// 每个物体都是完全相同的, 而且还在同一个位置, 我们只能看见一个物体
// 出于这个原因， GLSL在顶点着色器中嵌入了另一个内建变量, gl_InstanceID

// 在使用实例化渲染调用时, gl_InstanceID会从0开始, 在每个实例被渲染时递增1
// 比如说, 我们正在渲染第43个实例，那么顶点着色器中它的gl_InstanceID将会是42

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("1.vs", "1.fs");

    // generate a list of 100 quad locations/translation-vectors
    // ---------------------------------------------------------
    // 设置一个数组, 表示每一个绘制的四边形的偏移量
    shader.use();
    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2) {
        for (int x = -10; x < 10; x += 2) {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index] = translation;
            // 可以通过 uniform 的方式设置一个数组, 然后使用 gl_InstanceID 进行索引
            shader.setVec2(("offsets[" + std::to_string (index) + "]").c_str(), translations[index++]);
        }
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // 每一个多边形绘制的样式
    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    // 正常的生成 VAO,VBO, 绑定数据
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input 
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw 100 instanced quads
        shader.use();
        glBindVertexArray(quadVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}