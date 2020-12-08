#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
// 图像加载库
#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/stb_image.h>

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <cmath>
#include <stdlib.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800U;
const unsigned int SCR_HEIGHT = 800U;

// vertex shader, fragment shader
std::string VERTEX_SHADER("01.vert");
std::string FRAGMENT_SHADER("01.frag");
const char* TEXTURE_IMG[] = { "container.jpg", "awesomeface.png" };
int kind;
float mixRate = 0.2f;
#define MIX_RATE_DELTA 0.005f

int main(int argc, char *argv[]) {
    std::cout << "Args: " << argc << std::endl;
    if (argc == 1) {
        std::cout << "You can input a num(2-width) to choose shaders."
            << std::endl;
    } else {
        VERTEX_SHADER = std::string(argv[1]) + ".vert";
        FRAGMENT_SHADER = std::string(argv[1]) + ".frag";
    }
    // Q
    kind = 0;
    if (argc == 3) {
        kind = atoi(argv[2]);
    }
    // Q3 允许调整混合比例
    if (kind == 3) {
        VERTEX_SHADER = std::string("03.vert");
        FRAGMENT_SHADER = std::string("03.frag");
    }
    std::cout
        << "    vertex shader: " << VERTEX_SHADER
        << "\n    fragment shader: " << FRAGMENT_SHADER
        << "\n    textures:" << TEXTURE_IMG[0] << "," << TEXTURE_IMG[1]
        << std::endl;
    std::cout << "kind(" << kind << "):\n"
        << "    1:" << "four face.\n"
        << "    2:" << "part of face.\n"
        << "    3:" << "up/down to change mix rate."
        << std::endl;
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader shaders(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    float L = 0.0f, R = 1.0f;
    // Q1: 将此处的 texture coords 1.0f -> 2.0f (产生2*2个笑脸效果)
    if (kind == 1) {
        R = 2.0f;
    }
    // Q2: 只使用图片中间的纹理
    if (kind == 2) {
        L = 0.45f;
        R = 0.55f;
    }
    float vertices[] = {
         // positions         // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   R, R,  // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   R, L,  // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   L, L,  // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   L, R   // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices), indices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // deal with the texture starts
    GLuint texture[2];
    glGenTextures(2, texture);
    // load texture starts
    GLuint picFormat[] = { GL_RGB, GL_RGBA };
    // 上下翻转, 因为图片的坐标系 y 向下增长, opengl y 向上增长
    stbi_set_flip_vertically_on_load(true);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        int textureWidth, textureHeight, nrChannels;
        unsigned char *data = stbi_load(
            TEXTURE_IMG[i], &textureWidth, &textureHeight, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, picFormat[i],
                textureWidth, textureHeight,
                0, picFormat[i], GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture " << TEXTURE_IMG[i]
                << std::endl;
        }
        stbi_image_free(data); // free
    }
    // load texture ends
    // don't forget to activate/use the shader before setting uniforms!
    shaders.use(); 
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(shaders.ID, "texture1"), 0);
    // or set it via the texture class
    shaders.setInt("texture2", 1);

    // deal with the texture ends
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // seq, 发现上面只有 EBO = 2
    glBindTexture(GL_TEXTURE_2D, 0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render starts
        shaders.use();
        if (kind == 3) {
            shaders.setFloat("mixRate", mixRate);
        }
        // bind texture
        // glActiveTexture(GL_TEXTURE0); // GL_TEXTURE0 默认激活
        // GL_TEXTURE1 = GL_TEXTURE0 + 1
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        // unbind
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        // render ends
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(2, texture);
    shaders.clear();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (kind == 3) {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            mixRate += MIX_RATE_DELTA;
            if (mixRate >= 1.0f) {
                mixRate = 1.0f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            mixRate -= MIX_RATE_DELTA;
            if (mixRate <= 0.0f) {
                mixRate = 0.0f;
            }
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}