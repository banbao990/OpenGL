// Tutorial: https://learnopengl-cn.github.io/01%20Getting%20started/03%20Hello%20Window/
// Code: https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/1.2.hello_window_clear/hello_window_clear.cpp

// 需要首先包含 glad.h 头文件, 因为其他文件可能引用到它
// 函数指针头
#include <glad/glad.h>
// 窗口管理头
#include <GLFW/glfw3.h>

// 其他头文件
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int main() {
    // glfw 初始化
    glfwInit();
    // 配置 glfw 属性
    // 版本号 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // 核心模式 core
    // 在下载 glad 库的时候用到的 core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        // 对应于 glfwInit(), 释放资源
        glfwTerminate();
        return -1;
    }
    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);

    // 响应窗口大小被改变时的事件
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad 初始化
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 渲染的主循环
    // 检测当前窗口是否需要被关闭
    while (!glfwWindowShouldClose(window)) {
        // 响应按键输入的函数
        processInput(window);
        // 渲染 render

        // glClearColor 设置清空屏幕所使用的颜色
        // 状态设置函数
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        // 清空上一次渲染留下的缓冲
        // GL_COLOR_BUFFER_BIT(只清除颜色位)
        // 状态使用函数
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换缓冲, 显示更新图像(双缓冲中交换前后缓冲)
        glfwSwapBuffers(window);

        // 检查有没有触发什么事件(比如键盘输入, 鼠标移动等)
        // 更新窗口状态, 并调用对应的回调函数(可以通过回调方法手动设置)
        // 响应注册的回调函数
        glfwPollEvents();
    }
    glfwTerminate();
}

void processInput(GLFWwindow *window) {
    // 如果按下了 ESC 键, 那么将窗口的状态设置为需要关闭
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // 设置渲染窗口的大小
    // 左上角坐标, 大小
    glViewport(0, 0, width, height);
}