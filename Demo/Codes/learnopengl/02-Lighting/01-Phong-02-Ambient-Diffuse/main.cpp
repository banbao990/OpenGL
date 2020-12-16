#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
// 图像加载库
#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/stb_image.h>
// 矩阵计算库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 其他库
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <cmath>
#include <stdlib.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

unsigned int SCR_WIDTH = 800U;
unsigned int SCR_HEIGHT = 800U;

// vertex shader, fragment shader
std::string VERTEX_SHADER("01.vert");
std::string FRAGMENT_SHADER("01.frag");

namespace Camera {
    float theta = 0.0f, phi = 0.0f;
    const float DELTA = 1.0f;
    float zAngle = 0.0f;
    float fov = 45.0f;
}

namespace Light {
    float x = 0.8f, y = 0.8f, z = 0.8f;
    const float DELTA = 0.01f;
}

/*
 * diffuse + ambient 
 * az,sx,dc 调整光源位置
 */
int main(int argc, char *argv[]) {
    // std::cout << "Args: " << argc << std::endl;
    std::cout << "Move the light with x(a,z), y(s,x), z(d,c)!" << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 设置窗口回调函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // glfwSetKeyCallback(window, keyCallback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader shaders(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    Shader lightingShader("light.vert", "light.frag");
    // 简单的在数据中加入法向量(感觉怪怪的,同一个点有不同的法向)
    float vertices[] = {
        // position           // normal(单位向量)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    // light source
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // 只需要绑定 VBO 不用再次设置 VBO 的数据
    // 因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // 开启 z-buffer 深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // 清除上一轮保有的深度信息
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render starts
        shaders.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = glm::rotate(view, glm::radians(Camera::zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::rotate(view, glm::radians(Camera::theta), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, glm::radians(Camera::phi), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(
            glm::radians(Camera::fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaders.setTransformMatrix("model", model);
        shaders.setTransformMatrix("view", view);
        shaders.setTransformMatrix("projection", projection);
        shaders.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        shaders.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        // 不在GPU中计算是为了效率, 若在 GPU 中计算一次绘制需要计算多次 inverse
        // 需要将法线转化到世界坐标系中
        // 法线变换矩阵 G, 坐标变换矩阵 M
        // G = transpose(inverse(M))
        glm::mat4 matrixForNormal = glm::mat4(1.0);
        matrixForNormal = glm::transpose(glm::inverse(model));
        shaders.setMat4("matrixForNormal", matrixForNormal);
        // 设置光源位置
        glm::vec3 lightPos(Light::x, Light::y, Light::z);
        shaders.setVec3("lightPos", lightPos);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制光源
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightingShader.setMat4("model", model);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // unbind
        glBindVertexArray(0);
        // render ends
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    shaders.clear();
    lightingShader.clear();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // 上下左右键左右移动相机
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        Camera::phi += Camera::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        Camera::phi -= Camera::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        Camera::theta -= Camera::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        Camera::theta += Camera::DELTA;
    }
    // gimbal lock
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        Camera::zAngle -= Camera::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        Camera::zAngle += Camera::DELTA;
    }
    // light
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Light::x += Light::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        Light::x -= Light::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Light::y += Light::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        Light::y -= Light::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Light::z += Light::DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        Light::z -= Light::DELTA;
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

// scroll
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (Camera::fov >= 1.0f && Camera::fov <= 45.0f) {
        Camera::fov -= (float)yoffset;
    }
    if (Camera::fov <= 1.0f) {
        Camera::fov = 1.0f;
    }
    if (Camera::fov >= 45.0f) {
        Camera::fov = 45.0f;
    }
}

// resize the window
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

// mouse
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
    }
}