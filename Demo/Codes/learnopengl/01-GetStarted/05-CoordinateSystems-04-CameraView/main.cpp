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
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void cursorEnterCallback(GLFWwindow* window, int entered);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int SCR_WIDTH = 800U;
unsigned int SCR_HEIGHT = 800U;

// vertex shader, fragment shader
std::string VERTEX_SHADER("01.vert");
std::string FRAGMENT_SHADER("01.frag");
const char* TEXTURE_IMG = "awesomeface.png";

namespace Camera {
    // 相机属性
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // 考虑帧渲染时间, 使得相机匀速移动
    float cameraSpeed = 0.05f;
    float lastFrame = 0.0f;
    // 欧拉角
    float pitch = 0.0f, yaw = -90.0f, roll = 0.0f;
    // float phi = 0.0f, theta = 0.0f, zAngle = 0.0f;
    // 上一帧鼠标的位置
    double lastX = 400, lastY = 300;
    float sensitivity = 0.05f;
    // 前截面
    float fov = 45.0f;
}

/*
 * 通过(上下左右01)调整相机视角
 */
int main(int argc, char *argv[]) {
    // std::cout << "Args: " << argc << std::endl;
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
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // 设置窗口捕捉光标
    // 隐藏光标, 并捕捉(Capture)它
    // 捕捉光标表示的是, 如果焦点在你的程序上, 光标应该停留在窗口中 (除非程序失去焦点或者退出)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader shaders(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    float vertices[] = {
        // position + texture
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // deal with the texture starts
    GLuint texture;
    glGenTextures(1, &texture);
    // load texture starts
    GLuint picFormat = GL_RGBA;
    // 上下翻转, 因为图片的坐标系 y 向下增长, opengl y 向上增长
    stbi_set_flip_vertically_on_load(true);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int textureWidth, textureHeight, nrChannels;
    unsigned char *data = stbi_load(
        TEXTURE_IMG, &textureWidth, &textureHeight, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, picFormat,
            textureWidth, textureHeight,
            0, picFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << TEXTURE_IMG
            << std::endl;
    }
    stbi_image_free(data); // free
    // load texture ends
    // don't forget to activate/use the shader before setting uniforms!
    shaders.use();
    shaders.setInt("texture1", (GLint)0);

    // deal with the texture ends
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 开启 z-buffer 深度测试
    glEnable(GL_DEPTH_TEST);
    float xCamera = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // 获取帧渲染时间
        float currentFrame = (float)glfwGetTime();
        Camera::cameraSpeed = 2.5f*(currentFrame - Camera::lastFrame);
        Camera::lastFrame = currentFrame;
        // 清除上一轮保有的深度信息
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render starts
        shaders.use();
        // 正投影矩阵
        // glm::ortho(T left, T right, T bottom, T top, T zNear, T zFar);
        // 透视投影矩阵
        // glm::perspective(T fovy, T aspect, T zNear, T zFar)
        // glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
        // matrix model
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        // 随时间旋转
        // model = glm::rotate(model,
        // float)glfwGetTime() *
        // glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // matrix view 注意, 我们将矩阵向我们要进行移动场景的反方向移动
        // 这个问题很容易解释, 当前视点为(0,0,0), 如果要将视点移动到(x1,y1,z1)
        // 那么当前坐标系相对于新视点的坐标就变成了(x-x1,y-y1,z-z1)
        // 即做变换(-x1,-y1,-z1)
        glm::mat4 view = glm::mat4(1.0f);
        // 修改相机位置(注意顺序是相反的)
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // view = glm::rotate(view, glm::radians(Camera::zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        // view = glm::rotate(view, glm::radians(Camera::theta), glm::vec3(0.0f, 1.0f, 0.0f));
        // view = glm::rotate(view, glm::radians(Camera::phi), glm::vec3(1.0f, 0.0f, 0.0f));
        // lookAt 实现类似效果
        view = glm::lookAt(
            Camera::cameraPos,
            Camera::cameraPos + Camera::cameraFront,
            Camera::cameraUp
        );
        // camera position, center, camera' z axis
        // view = glm::translate(view,
        // glm::vec3(-Camera::theta, -Camera::phi, -3.0f));
        // matrix projection
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(
            glm::radians(Camera::fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaders.setTransformMatrix("model", model);
        shaders.setTransformMatrix("view", view);
        shaders.setTransformMatrix("projection", projection);

        // bind texture
        // glActiveTexture(GL_TEXTURE0); // GL_TEXTURE0 默认激活
        // GL_TEXTURE1 = GL_TEXTURE0 + 1
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // unbind
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        // render ends
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    shaders.clear();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // 上下左右键左右移动相机
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        Camera::cameraPos += Camera::cameraSpeed * Camera::cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        Camera::cameraPos -= Camera::cameraSpeed * Camera::cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        Camera::cameraPos -= Camera::cameraSpeed *
            glm::normalize(glm::cross(Camera::cameraFront, Camera::cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        Camera::cameraPos += Camera::cameraSpeed *
            glm::normalize(glm::cross(Camera::cameraFront, Camera::cameraUp));
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

// scroll
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (Camera::fov >= 1.0f && Camera::fov <= 45.0f) {
        Camera::fov -= yoffset;
    }
    if (Camera::fov <= 1.0f) {
        Camera::fov = 1.0f;
    }
    if (Camera::fov >= 45.0f) {
        Camera::fov = 45.0f;
    }
}

// cursor enter
void cursorEnterCallback(GLFWwindow* window, int entered) {
    if (entered) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        Camera::lastX = xPos;
        Camera::lastY = yPos;
    }
}

// cursor
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // 1. 计算鼠标距上一帧的偏移量
    float xoffset = (float)(xpos - Camera::lastX);
    float yoffset = (float)(Camera::lastY - ypos);
    Camera::lastX = xpos;
    Camera::lastY = ypos;
    xoffset *= Camera::sensitivity;
    yoffset *= Camera::sensitivity;

    // 2. 把偏移量添加到摄像机的俯仰角和偏航角中
    Camera::yaw += xoffset;
    Camera::pitch += yoffset;

    // 3. 对偏航角和俯仰角进行最大和最小值的限制
    if (Camera::pitch > 89.0f) {
        Camera::pitch = 89.0f;
    }
    if (Camera::pitch < -89.0f) {
        Camera::pitch = -89.0f;
    }
    // 4. 计算方向向量
    glm::vec3 front;
    front.x = cos(glm::radians(Camera::pitch)) * cos(glm::radians(Camera::yaw));
    front.y = sin(glm::radians(Camera::pitch));
    front.z = cos(glm::radians(Camera::pitch)) * sin(glm::radians(Camera::yaw));
    Camera::cameraFront = glm::normalize(front);
}