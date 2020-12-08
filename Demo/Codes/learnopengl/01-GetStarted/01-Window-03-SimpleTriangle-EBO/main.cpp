// Tutorial: https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
// Code: https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.1.hello_triangle/hello_triangle.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

#define GET_SHADER_FROME_FILE true

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
GLuint loadShaderSource(const char *path, GLenum type);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // GLSL: OpenGL Shading Language

    // VAO: vertex array object
    // VBO: vertex buffer object
    // EBO/IBO: Element buffer object/Index buffer object

    // 编译和使用 shader
    // 一些 log
    int success;
    char infoLog[512];
    // vertex shader
    GLuint vertexShader;
    // 获取来源
    if (GET_SHADER_FROME_FILE) {
        // 从文件中读取
        vertexShader = loadShaderSource("vertexShader1.vert", GL_VERTEX_SHADER);
    } else {
        // 这里直接存储在一个字符串中
        // 生成一个 vertex shader(绑定一个 ID)
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        // 获取 shader 代码
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        // 动态编译 shader 代码
        glCompileShader(vertexShader);
        // 查看是否存在编译错误
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            // 获取错误信息
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
    // fragment shader
    GLuint fragmentShader;
    if (GET_SHADER_FROME_FILE) {
        // 从文件中读取
        fragmentShader = loadShaderSource("fragmentShader1.frag", GL_FRAGMENT_SHADER);
    } else {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
    // 链接 shader 为一个 program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // 当链接成功之后, 我们就可以删除之前的 shader 对象了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 设置结点属性
    // 注意在 OpenGL 显示的时候经过了标准化, 只会显示 [-1.0, 1.0] 区间的像素
    // NDC: Normalized Device Coordinates

    // 给出数组(相当于给出一段连续的空间)
    float vertices[] = {
        // right
        0.25f, 0.5f, 0.0f,
        0.25f, -0.5f, 0.0f,
        0.75f, -0.5f, 0.0f,
        0.75f,  0.5f, 0.0f,
        // left
        -0.75f, 0.5f, 0.0f,
        -0.75f, -0.5f, 0.0f,
        -0.25f, -0.5f, 0.0f,
        -0.25f,  0.5f, 0.0f,
    };
    // 索引数组
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
        4, 5, 7,
        5, 6, 7
    };
    // EBO 保存一个索引
    GLuint VBO, VAO, EBO;
    // "1" 表示生成的个数, 会给 VAO 生成一个 ID
    // 一个其他的例子
    // GLuint vbo[3];
    // glGenBuffers(3, vbo);
    glGenVertexArrays(1, &VAO);
    // VBO 生成一个 ID
    glGenBuffers(1, &VBO);
    // EBO 生成一个 ID
    glGenBuffers(1, &EBO);

    // 顺序:
    // (1) 绑定 VAO
    glBindVertexArray(VAO);
    // (2) 绑定和设置 VBO
    // 将 VBO 和 GL_ARRAY_BUFFER 进行绑定, 之后对 GL_ARRAY_BUFFER 的操作就是对 VBO 的操作
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // GL_STATIC_DRAW : 数据不会或几乎不会改变
    // GL_DYNAMIC_DRAW: 数据会被改变很多
    // GL_STREAM_DRAW : 数据每次绘制时都会改变
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 绑定和设置 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // (3) 设置 OpenGL 读取数据方式(链接顶点属性)
    // 参数说明
    //     0:                   和 vertex shader 中的 location = 0 对应
    //     3:                   顶点属性的大小
    //     GL_FLOAT:            指定数据的类型
    //     GL_FALSE:            重新进行标准化映射 (uint -> [0, 1]; int -> [-1, 1])
    //     3 * sizeof(float):   步长(每个数据大小) stride
    //     (void*)):            数据在缓冲区中的偏移量 offset
    // 具体绑定的 VBO 是最近绑定到 GL_ARRAY_BUFFER 的 VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 以顶点属性位置值作为参数, 启用顶点属性(默认不启用)
    // 不启用, 即意味着数据在着色器端是不可见的(但是数据已经通过 glVertexAttribPointer 传到 GPU 了)
    glEnableVertexAttribArray(0);

    // 将 GL_ARRAY_BUFFER 与其他 VBO 绑定
    // 0, 为了之后安全的解绑, 也为了防止意外修改原来的 VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 类似的, 解绑之前的 VAO
    glBindVertexArray(0);
    // EBO 的绑定信息保存在 VAO 之中
    // 如果在 VAO 还没解绑的时候重新绑定, 就修改了 VAO 中保存的 EBO 索引, 这样就出大问题
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 使用原来的 shaders 绑定的 Program
        glUseProgram(shaderProgram);
        // 重新绑定原来的 VAO
        glBindVertexArray(VAO);
        // 绘制三角形
        // 线框
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        // 填充
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 6));
        // 解绑(但是由于这个程序比较简单, 这个步骤倒不是很重要)
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

GLuint loadShaderSource(const char *path, GLenum type) {
    GLuint shader = glCreateShader(type);

    std::ifstream file(path);
    std::string sourceString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    const char *sourcePointers[1] = { sourceString.c_str() };

    glShaderSource(shader, 1, sourcePointers, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    // check for shader compile errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\nPATH:" << path << "\n" << infoLog << std::endl;
    }

    return shader;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}