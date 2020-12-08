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

    // �����ʹ�� shader
    // һЩ log
    int success;
    char infoLog[512];
    // vertex shader
    GLuint vertexShader;
    // ��ȡ��Դ
    if (GET_SHADER_FROME_FILE) {
        // ���ļ��ж�ȡ
        vertexShader = loadShaderSource("vertexShader1.vert", GL_VERTEX_SHADER);
    } else {
        // ����ֱ�Ӵ洢��һ���ַ�����
        // ����һ�� vertex shader(��һ�� ID)
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        // ��ȡ shader ����
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        // ��̬���� shader ����
        glCompileShader(vertexShader);
        // �鿴�Ƿ���ڱ������
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            // ��ȡ������Ϣ
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
    // fragment shader
    GLuint fragmentShader;
    if (GET_SHADER_FROME_FILE) {
        // ���ļ��ж�ȡ
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
    // ���� shader Ϊһ�� program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // ������Ӵ���
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // �����ӳɹ�֮��, ���ǾͿ���ɾ��֮ǰ�� shader ������
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ���ý������
    // ע���� OpenGL ��ʾ��ʱ�򾭹��˱�׼��, ֻ����ʾ [-1.0, 1.0] ���������
    // NDC: Normalized Device Coordinates

    // ��������(�൱�ڸ���һ�������Ŀռ�)
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
    // ��������
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
        4, 5, 7,
        5, 6, 7
    };
    // EBO ����һ������
    GLuint VBO, VAO, EBO;
    // "1" ��ʾ���ɵĸ���, ��� VAO ����һ�� ID
    // һ������������
    // GLuint vbo[3];
    // glGenBuffers(3, vbo);
    glGenVertexArrays(1, &VAO);
    // VBO ����һ�� ID
    glGenBuffers(1, &VBO);
    // EBO ����һ�� ID
    glGenBuffers(1, &EBO);

    // ˳��:
    // (1) �� VAO
    glBindVertexArray(VAO);
    // (2) �󶨺����� VBO
    // �� VBO �� GL_ARRAY_BUFFER ���а�, ֮��� GL_ARRAY_BUFFER �Ĳ������Ƕ� VBO �Ĳ���
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // GL_STATIC_DRAW : ���ݲ���򼸺�����ı�
    // GL_DYNAMIC_DRAW: ���ݻᱻ�ı�ܶ�
    // GL_STREAM_DRAW : ����ÿ�λ���ʱ����ı�
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // �󶨺����� EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // (3) ���� OpenGL ��ȡ���ݷ�ʽ(���Ӷ�������)
    // ����˵��
    //     0:                   �� vertex shader �е� location = 0 ��Ӧ
    //     3:                   �������ԵĴ�С
    //     GL_FLOAT:            ָ�����ݵ�����
    //     GL_FALSE:            ���½��б�׼��ӳ�� (uint -> [0, 1]; int -> [-1, 1])
    //     3 * sizeof(float):   ����(ÿ�����ݴ�С) stride
    //     (void*)):            �����ڻ������е�ƫ���� offset
    // ����󶨵� VBO ������󶨵� GL_ARRAY_BUFFER �� VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // �Զ�������λ��ֵ��Ϊ����, ���ö�������(Ĭ�ϲ�����)
    // ������, ����ζ����������ɫ�����ǲ��ɼ���(���������Ѿ�ͨ�� glVertexAttribPointer ���� GPU ��)
    glEnableVertexAttribArray(0);

    // �� GL_ARRAY_BUFFER ������ VBO ��
    // 0, Ϊ��֮��ȫ�Ľ��, ҲΪ�˷�ֹ�����޸�ԭ���� VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // ���Ƶ�, ���֮ǰ�� VAO
    glBindVertexArray(0);
    // EBO �İ���Ϣ������ VAO ֮��
    // ����� VAO ��û����ʱ�����°�, ���޸��� VAO �б���� EBO ����, �����ͳ�������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ʹ��ԭ���� shaders �󶨵� Program
        glUseProgram(shaderProgram);
        // ���°�ԭ���� VAO
        glBindVertexArray(VAO);
        // ����������
        // �߿�
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        // ���
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 6));
        // ���(���������������Ƚϼ�, ������赹���Ǻ���Ҫ)
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