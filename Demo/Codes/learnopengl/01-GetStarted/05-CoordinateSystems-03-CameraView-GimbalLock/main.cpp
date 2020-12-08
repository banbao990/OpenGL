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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int SCR_WIDTH = 800U;
unsigned int SCR_HEIGHT = 800U;

// vertex shader, fragment shader
std::string VERTEX_SHADER("01.vert");
std::string FRAGMENT_SHADER("01.frag");
const char* TEXTURE_IMG = "awesomeface.png";

namespace Camera {
    float theta = 0.0f, phi = 0.0f;
    const float DELTA = 1.0f;
    float zAngle = 0.0f;
    float fov = 45.0f;
}

/*
 * 通过(上下左右01)调整相机视角, 滚轮调远近
 * 万向锁
 * 产生万向锁的一种方式: 右90度
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scrollCallback);
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
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        // 随时间旋转
        // model = glm::rotate(model,
            // float)glfwGetTime() * 
            // glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // matrix view 注意, 我们将矩阵向我们要进行移动场景的反方向移动
        // 这个问题很容易解释, 当前视点为(x,y,z), 如果要将视点移动到(x1,y1,z1)
        // 那么当前坐标系相对于新视点的坐标就变成了(x-x1,y-y1,z-z1)
        // 即做变换(-x1,-y1,-z1)
        glm::mat4 view = glm::mat4(1.0f);
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // 修改相机位置(注意顺序是相反的)
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = glm::rotate(view, glm::radians(Camera::zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::rotate(view, glm::radians(Camera::theta), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, glm::radians(Camera::phi), glm::vec3(1.0f, 0.0f, 0.0f));
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
}

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
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

// mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
    }
}