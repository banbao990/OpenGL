#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
// ͼ����ؿ�
#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/stb_image.h>
// ��������
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ������
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
    // �������
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // ����֡��Ⱦʱ��, ʹ����������ƶ�
    float cameraSpeed = 0.05f;
    float lastFrame = 0.0f;
    // ŷ����
    float pitch = 0.0f, yaw = -90.0f, roll = 0.0f;
    // float phi = 0.0f, theta = 0.0f, zAngle = 0.0f;
    // ��һ֡����λ��
    double lastX = 400, lastY = 300;
    float sensitivity = 0.05f;
    // ǰ����
    float fov = 45.0f;
}

/*
 * ͨ��(��������01)��������ӽ�
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
    // ���ô��ڻص�����
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // ���ô��ڲ�׽���
    // ���ع��, ����׽(Capture)��
    // ��׽����ʾ����, �����������ĳ�����, ���Ӧ��ͣ���ڴ����� (���ǳ���ʧȥ��������˳�)
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
    // ���·�ת, ��ΪͼƬ������ϵ y ��������, opengl y ��������
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
    // ���� z-buffer ��Ȳ���
    glEnable(GL_DEPTH_TEST);
    float xCamera = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // ��ȡ֡��Ⱦʱ��
        float currentFrame = (float)glfwGetTime();
        Camera::cameraSpeed = 2.5f*(currentFrame - Camera::lastFrame);
        Camera::lastFrame = currentFrame;
        // �����һ�ֱ��е������Ϣ
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render starts
        shaders.use();
        // ��ͶӰ����
        // glm::ortho(T left, T right, T bottom, T top, T zNear, T zFar);
        // ͸��ͶӰ����
        // glm::perspective(T fovy, T aspect, T zNear, T zFar)
        // glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
        // matrix model
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        // ��ʱ����ת
        // model = glm::rotate(model,
        // float)glfwGetTime() *
        // glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // matrix view ע��, ���ǽ�����������Ҫ�����ƶ������ķ������ƶ�
        // �����������׽���, ��ǰ�ӵ�Ϊ(0,0,0), ���Ҫ���ӵ��ƶ���(x1,y1,z1)
        // ��ô��ǰ����ϵ��������ӵ������ͱ����(x-x1,y-y1,z-z1)
        // �����任(-x1,-y1,-z1)
        glm::mat4 view = glm::mat4(1.0f);
        // �޸����λ��(ע��˳�����෴��)
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // view = glm::rotate(view, glm::radians(Camera::zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        // view = glm::rotate(view, glm::radians(Camera::theta), glm::vec3(0.0f, 1.0f, 0.0f));
        // view = glm::rotate(view, glm::radians(Camera::phi), glm::vec3(1.0f, 0.0f, 0.0f));
        // lookAt ʵ������Ч��
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
        // glActiveTexture(GL_TEXTURE0); // GL_TEXTURE0 Ĭ�ϼ���
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
    // �������Ҽ������ƶ����
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
    // 1. ����������һ֡��ƫ����
    float xoffset = (float)(xpos - Camera::lastX);
    float yoffset = (float)(Camera::lastY - ypos);
    Camera::lastX = xpos;
    Camera::lastY = ypos;
    xoffset *= Camera::sensitivity;
    yoffset *= Camera::sensitivity;

    // 2. ��ƫ������ӵ�������ĸ����Ǻ�ƫ������
    Camera::yaw += xoffset;
    Camera::pitch += yoffset;

    // 3. ��ƫ���Ǻ͸����ǽ���������Сֵ������
    if (Camera::pitch > 89.0f) {
        Camera::pitch = 89.0f;
    }
    if (Camera::pitch < -89.0f) {
        Camera::pitch = -89.0f;
    }
    // 4. ���㷽������
    glm::vec3 front;
    front.x = cos(glm::radians(Camera::pitch)) * cos(glm::radians(Camera::yaw));
    front.y = sin(glm::radians(Camera::pitch));
    front.z = cos(glm::radians(Camera::pitch)) * sin(glm::radians(Camera::yaw));
    Camera::cameraFront = glm::normalize(front);
}