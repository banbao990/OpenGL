// Tutorial: https://learnopengl-cn.github.io/01%20Getting%20started/03%20Hello%20Window/
// Code: https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/1.2.hello_window_clear/hello_window_clear.cpp

// ��Ҫ���Ȱ��� glad.h ͷ�ļ�, ��Ϊ�����ļ��������õ���
// ����ָ��ͷ
#include <glad/glad.h>
// ���ڹ���ͷ
#include <GLFW/glfw3.h>

// ����ͷ�ļ�
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int main() {
    // glfw ��ʼ��
    glfwInit();
    // ���� glfw ����
    // �汾�� 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // ����ģʽ core
    // ������ glad ���ʱ���õ��� core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ����һ�����ڶ���
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        // ��Ӧ�� glfwInit(), �ͷ���Դ
        glfwTerminate();
        return -1;
    }
    // �����ڵ�����������Ϊ��ǰ�̵߳���������
    glfwMakeContextCurrent(window);

    // ��Ӧ���ڴ�С���ı�ʱ���¼�
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad ��ʼ��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ��Ⱦ����ѭ��
    // ��⵱ǰ�����Ƿ���Ҫ���ر�
    while (!glfwWindowShouldClose(window)) {
        // ��Ӧ��������ĺ���
        processInput(window);
        // ��Ⱦ render

        // glClearColor ���������Ļ��ʹ�õ���ɫ
        // ״̬���ú���
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        // �����һ����Ⱦ���µĻ���
        // GL_COLOR_BUFFER_BIT(ֻ�����ɫλ)
        // ״̬ʹ�ú���
        glClear(GL_COLOR_BUFFER_BIT);

        // ��������, ��ʾ����ͼ��(˫�����н���ǰ�󻺳�)
        glfwSwapBuffers(window);

        // �����û�д���ʲô�¼�(�����������, ����ƶ���)
        // ���´���״̬, �����ö�Ӧ�Ļص�����(����ͨ���ص������ֶ�����)
        // ��Ӧע��Ļص�����
        glfwPollEvents();
    }
    glfwTerminate();
}

void processInput(GLFWwindow *window) {
    // ��������� ESC ��, ��ô�����ڵ�״̬����Ϊ��Ҫ�ر�
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // ������Ⱦ���ڵĴ�С
    // ���Ͻ�����, ��С
    glViewport(0, 0, width, height);
}