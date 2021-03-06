#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void getMaximumNR();
GLuint loadShaderSource(const char *path, GLenum type);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// vertex shader, fragment shader
std::string VERTEX_SHADER("vs.vert");
std::string FRAGMENT_SHADER("fs.frag");

int main(int argc, char *argv[]) {
    // args
    std::cout << argc << " args" << std::endl;
    if (argc == 1) {
        std::cout << "You can input a num(2-width) to choose shaders" 
            << std::endl;
    } else {
        VERTEX_SHADER = "vs" + std::string(argv[1]) + ".vert";
        FRAGMENT_SHADER = "fs" + std::string(argv[1]) + ".frag";
    }
    std::cout
        << "vertex shader: " << VERTEX_SHADER << std::endl
        << "fragment shader: " << FRAGMENT_SHADER << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    int success;
    char infoLog[512];
    GLuint vertexShader;
    vertexShader = loadShaderSource(VERTEX_SHADER.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader;
    fragmentShader = loadShaderSource(FRAGMENT_SHADER.c_str(), GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    float vertices[] = {
        -0.75f, -0.75f, 0.0f,
        -0.5f, -0.25f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.5f, -0.25f, 0.0f,
        0.25f, -0.75f, 0.0f,
        0.75f, 0.25f, 0.0f,
        0.5f, 0.75f, 0.0f,
        0.25f, 0.25f, 0.0f,
        -0.75f, 0.25f, 0.0f,
        -0.5f, 0.75f, 0.0f,
        -0.25f, 0.25f, 0.0f,
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render starts
        glUseProgram(shaderProgram);
        // uniform starts
        float timeValue = (float)glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        // uniform ends
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 3, 6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 6, 9);
        glDrawArrays(GL_TRIANGLES, 9, 12);
        glBindVertexArray(0);
        // render ends
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
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

// 需要在配置好 opengl 的环境之后才能调用, 不然会抛出异常
// output: 16
void getMaximumNR() {
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: "
        << nrAttributes << std::endl;
}