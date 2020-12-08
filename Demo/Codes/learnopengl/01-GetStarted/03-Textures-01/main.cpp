#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
// ͼ����ؿ�
#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/stb_image.h>

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
GLuint loadShaderSource(const char *path, GLenum type);

const unsigned int SCR_WIDTH = 800U;
const unsigned int SCR_HEIGHT = 800U;

// vertex shader, fragment shader
std::string VERTEX_SHADER("vs01.vert");
std::string FRAGMENT_SHADER("fs01.frag");
std::string TEXTURE_IMG("container.jpg");

int main(int argc, char *argv[]) {
    std::cout << "Demo.exe vs/fs(01) texture(container.jpg)" << std::endl;
    // args
    std::cout << argc << " args" << std::endl;
    if (argc == 1) {
        std::cout << "You can input a num(2-width) to choose shaders."
            << std::endl;
    } else {
        VERTEX_SHADER = "vs" + std::string(argv[1]) + ".vert";
        FRAGMENT_SHADER = "fs" + std::string(argv[1]) + ".frag";
    }
    std::cout
        << "    vertex shader: " << VERTEX_SHADER
        << "\n    fragment shader: " << FRAGMENT_SHADER
        << std::endl;

    // load texture starts
    if (argc == 3) {
        TEXTURE_IMG = std::string(argv[2]);
    } else {
        std::cout << "You can input a file to choose texture."
            << std::endl;
    }
    int textureWidth, textureHeight, nrChannels;
    unsigned char *data = stbi_load(
        TEXTURE_IMG.c_str(),
        &textureWidth, &textureHeight, &nrChannels, 0);
    if (data) {
        std::cout << "    texture: " << TEXTURE_IMG
            << "\n    size: (" << textureHeight << "*" << textureWidth << ")"
            << "\n    channels: " << nrChannels
            << std::endl;
    } else {
        std::cout << "Can't load the texture " << TEXTURE_IMG << std::endl;
    }
    // load texture ends

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
    // more data
    float vertices[] = {
         // positions         // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
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
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // GL_REPEAT:          �������Ĭ����Ϊ, �ظ�����ͼ��
    // GL_MIRRORED_REPEAT: �ظ�ͼƬ�Ǿ�����õ�
    // GL_CLAMP_TO_EDGE:   �����Ĳ��ֻ��ظ���������ı�Ե, ��Ե����Ч��
    // GL_CLAMP_TO_BORDER: ����������Ϊ�û�ָ���ı�Ե��ɫ
    // float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // С�����Ӧ��ͼ��ʱ����䷽ʽ Minify
    // GL_LINEAR:  ���Բ�ֵ
    // GL_NEAREST: ����ڲ�ֵ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // �������ӦСͼ��ʱ����䷽ʽ Magnify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // �༶��Զ����
    // GL_ƥ�����ش�С��ʽ_MIPMAP_���������ʽ
    // GL_NEAREST_MIPMAP_NEAREST
    //      ʹ�����ڽ��Ķ༶��Զ������ƥ�����ش�С, ��ʹ���ڽ���ֵ�����������
    // GL_LINEAR_MIPMAP_NEAREST
    //      ʹ�����ڽ��Ķ༶��Զ������, ��ʹ�����Բ�ֵ���в���
    // GL_NEAREST_MIPMAP_LINEAR
    //      ��������ƥ�����ش�С�Ķ༶��Զ����֮��������Բ�ֵ, ʹ���ڽ���ֵ���в���
    // GL_LINEAR_MIPMAP_LINEAR
    //      �������ڽ��Ķ༶��Զ����֮��ʹ�����Բ�ֵ, ��ʹ�����Բ�ֵ���в���
    if (data) {
        // ��һ������ָ��������Ŀ��(Target)
        //     ����ΪGL_TEXTURE_2D�������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ�����
        //     (�κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ��)
        // �ڶ�������Ϊ����ָ���༶��Զ����ļ���
        //     �����ϣ�������ֶ�����ÿ���༶��Զ����ļ���Ļ�
        //     ����������0, Ҳ���ǻ�������
        // �������������� OpenGL ����ϣ����������Ϊ���ָ�ʽ
        //     ���ǵ�ͼ��ֻ��RGBֵ, �������Ҳ��������ΪRGBֵ��
        // ���ĸ��͵���������������յ�����Ŀ�Ⱥ͸߶�
        //     ����֮ǰ����ͼ���ʱ�򴢴�������, ��������ʹ�ö�Ӧ�ı���
        // �¸�����Ӧ�����Ǳ���Ϊ0(��ʷ����������)
        // ���ߵڰ˸�����������Դͼ�ĸ�ʽ����������
        //     ����ʹ��RGBֵ�������ͼ��, �������Ǵ���Ϊchar(byte)����, 
        //     ���ǽ��ᴫ���Ӧֵ
        // ���һ��������������ͼ������
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight,
            0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // ��������
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // deal with the texture ends
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // seq
    stbi_image_free(data); // free
    // shader.setInt("ourTexture", 0); // ������ʱ����Ҫ��
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render starts
        shaders.use();

        // bind texture
        // glActiveTexture(GL_TEXTURE0); GL_TEXTURE0 Ĭ�ϼ���
        glBindTexture(GL_TEXTURE_2D, texture);
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
    shaders.clear();
    glfwTerminate();
    return 0;
}

GLuint loadShaderSource(const char *path, GLenum type) {
    GLuint shader = glCreateShader(type);
    std::ifstream file(path);
    std::string sourceString((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    const char *sourcePointers[1] = { sourceString.c_str() };
    glShaderSource(shader, 1, sourcePointers, NULL);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout
            << "ERROR::SHADER::COMPILATION_FAILED\nPATH:" << path << "\n"
            << infoLog << std::endl;
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