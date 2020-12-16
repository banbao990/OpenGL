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

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void readMaterialAndLight(const char *materialPath, const char *lightPath);

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

const float LIGHT_DELTA = 0.01f;
Light light;
Material material;

/*
 * material
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
    
    light.position = glm::vec3(0.8f, 0.8f, 0.8f);
    readMaterialAndLight("material.txt", "light.txt");
    while (!glfwWindowShouldClose(window)) {
        // 清除上一轮保有的深度信息
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 处理窗口信息
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render starts
        shaders.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        glm::mat4 view = glm::mat4(1.0f);
        glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
        // 注意变换顺序是倒着来的, 这是源于 opengl 列优先存储
        // 将实际的变换矩阵记作0, opengl 中的矩阵记作1, t(X) = transpose(X)
        // M1 = t(M0)
        // R0 = T1*T2*...*Tn*M0
        // =>
        // R1 = t(R0) = M1*t(Tn)*...*t(T2)*t(T1)
        // 注意移动方向和视点方向相反
        view = glm::translate(view, -viewPos);
        // 以下为什么不需要负号
        // 0. glm::rotate 是逆时针旋转
        // 1. 原来是需要将视点进行 (phi,theta,zAngle) 变换
        // 2. 等价于将所有物体做 (-phi, -theta, -zAngle) 变换
        // 3. 逆时针: rotate(phi, theta, zAngle)
        view = glm::rotate(view, glm::radians(Camera::zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::rotate(view, glm::radians(Camera::theta), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, glm::radians(Camera::phi), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(
            glm::radians(Camera::fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaders.setTransformMatrix("model", model);
        shaders.setTransformMatrix("view", view);
        shaders.setTransformMatrix("projection", projection);
        glm::mat4 matrixForNormal = glm::mat4(1.0);
        // 修改为是在观察空间中计算光照效果
        matrixForNormal = glm::transpose(glm::inverse(view * model));
        shaders.setMat4("matrixForNormal", matrixForNormal);
        // 设置光源位置, 注意光源也要变换到观察坐标系中
        // lightPos 是世界坐标
        // lightPos4 是观察坐标
        glm::vec4 lightPos4(light.position, 1.0f);
        lightPos4 = view * lightPos4;
        shaders.setVec3("light.position", glm::vec3(
            lightPos4.x / lightPos4.w,
            lightPos4.y / lightPos4.w,
            lightPos4.z / lightPos4.w
        ));
        shaders.setVec3("light.ambient", light.ambient);
        shaders.setVec3("light.diffuse", light.diffuse);
        shaders.setVec3("light.specular", light.specular);
        // 设置相机位置(注意这里是观察坐标系)
        shaders.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 0.0f));
        
        // material
        shaders.setVec3("material.ambient", material.ambient);
        shaders.setVec3("material.diffuse", material.diffuse);
        shaders.setVec3("material.specular", material.specular);
        shaders.setFloat("material.shininess", material.shininess);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制光源
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, light.position);
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

void readMaterialAndLight(const char *materialPath, const char *lightPath) {
    std::ifstream materialFile;
    std::ifstream lightFile;
    // ensure ifstream objects can throw exceptions:
    materialFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    lightFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        materialFile.open(materialPath);
        lightFile.open(lightPath);
        // read start
        // material
        for (int i = 0; i < 3; ++i) { materialFile >> material.ambient[i]; }
        for (int i = 0; i < 3; ++i) { materialFile >> material.diffuse[i]; }
        for (int i = 0; i < 3; ++i) { materialFile >> material.specular[i]; }
        materialFile >> material.shininess;
        // light
        for (int i = 0; i < 3; ++i) { lightFile >> light.ambient[i]; }
        for (int i = 0; i < 3; ++i) { lightFile >> light.diffuse[i]; }
        for (int i = 0; i < 3; ++i) { lightFile >> light.specular[i]; }
        // read end
        // close file handlers
        materialFile.close();
        lightFile.close();
    } catch (std::ifstream::failure& e) {
        std::cout
            << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n"
            << "ERROR CODE: " << e.code() << "\n"
            << "WHAT: " << e.what()
            << std::endl;
    }
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
        light.position.x += LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        light.position.x -= LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        light.position.y += LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        light.position.y -= LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        light.position.z += LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        light.position.z -= LIGHT_DELTA;
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