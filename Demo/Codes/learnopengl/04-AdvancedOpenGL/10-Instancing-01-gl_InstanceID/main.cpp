
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/shader.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/* instancing ʵ���� */
// ��������һ�������˺ܶ�ģ�͵ĳ���, ���󲿷ֵ�ģ�Ͱ�������ͬһ�鶥������
// ֻ�������е��ǲ�ͬ������ռ�任
// ����һ�������ݵĳ���: ÿ���ݶ���һ���������������ε�Сģ��, ����ܻ���Ҫ���ƺܶ����
// ������ÿ֡������ܻ���Ҫ��Ⱦ��ǧ�����������
// ��Ϊÿһ���ݽ������ɼ��������ι���, ��Ⱦ������˲����ɵ�
// ����ǧ����Ⱦ��������ȴ�Ἣ���Ӱ������

// ���ܲ���ƿ����ԭ��
/*
for() {
    DoSomePreparations(); // �� VAO, ������, ���� uniform ��
    glDrawArrays(GL_TRIANGLES, 0, amount_of_vertices);
}
*/
// �������������ģ�͵Ĵ���ʵ��(Instance), ��ܿ�ͻ���Ϊ���Ƶ��ù�����ﵽ����ƿ��
// ����ƶ��㱾�����, ʹ�� glDrawArrays �� glDrawElements �������� GPU ȥ
// ������Ķ������ݻ����ĸ��������
// ��Ϊ OpenGL �ڻ��ƶ�������֮ǰ��Ҫ���ܶ�׼������(�������GPU�ô��ĸ������ȡ����, 
// ����Ѱ�Ҷ�������, ������Щ��������Ի�����CPU��GPU����(CPU to GPU Bus)�Ͻ��е�)
// ����, ������Ⱦ����ǳ���, ���� GPU ȥ��Ⱦȴδ��

// ��������ܹ�������һ���Է��͸�GPU
// Ȼ��ʹ��һ�����ƺ����� OpenGL ������Щ���ݻ��ƶ������, �ͻ��������
// �����ʵ����(Instancing)

// ʵ����������ܹ�������ʹ��һ����Ⱦ���������ƶ������
// ����ʡÿ�λ�������ʱCPU -> GPU��ͨ��, ��ֻ��Ҫһ�μ���

// ʵ������Ⱦ
// glDrawArrays, glDrawElements 
//    => glDrawArraysInstanced, glDrawElementsInstanced
// ��Щ��Ⱦ������ʵ�����汾��Ҫһ������Ĳ���
// ����ʵ������(Instance Count), ���ܹ�����������Ҫ��Ⱦ��ʵ������

// �����������û��ʲô��, ��Ⱦͬһ������һǧ�ζ����ǲ�û��ʲô�ô�
// ÿ�����嶼����ȫ��ͬ��, ���һ���ͬһ��λ��, ����ֻ�ܿ���һ������
// �������ԭ�� GLSL�ڶ�����ɫ����Ƕ������һ���ڽ�����, gl_InstanceID

// ��ʹ��ʵ������Ⱦ����ʱ, gl_InstanceID���0��ʼ, ��ÿ��ʵ������Ⱦʱ����1
// ����˵, ����������Ⱦ��43��ʵ������ô������ɫ��������gl_InstanceID������42

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("1.vs", "1.fs");

    // generate a list of 100 quad locations/translation-vectors
    // ---------------------------------------------------------
    // ����һ������, ��ʾÿһ�����Ƶ��ı��ε�ƫ����
    shader.use();
    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2) {
        for (int x = -10; x < 10; x += 2) {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index] = translation;
            // ����ͨ�� uniform �ķ�ʽ����һ������, Ȼ��ʹ�� gl_InstanceID ��������
            shader.setVec2(("offsets[" + std::to_string (index) + "]").c_str(), translations[index++]);
        }
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // ÿһ������λ��Ƶ���ʽ
    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    // ���������� VAO,VBO, ������
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input 
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw 100 instanced quads
        shader.use();
        glBindVertexArray(quadVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}