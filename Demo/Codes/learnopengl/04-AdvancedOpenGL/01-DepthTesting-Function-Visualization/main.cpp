#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

/* 1. ��Ȳ��� depth testing */
// ����Ȳ��Ա����õ�ʱ��, OpenGL�Ὣһ��Ƭ�ε����ֵ����Ȼ�������ݽ��жԱ�
// OpenGL��ִ��һ����Ȳ���, ����������ͨ���˵Ļ�, ��Ȼ��彫�����Ϊ�µ����ֵ
// �����Ȳ���ʧ����, Ƭ�ν��ᱻ���� 

/* 2. ��Ȳ���ʱ�� */
// Ƭ����ɫ������֮��(�Լ�ģ�����(Stencil Testing)����֮��)

/* 3. ���� */
// ��Ȳ���Ĭ���ǽ��õ�, ����:
// glEnable(GL_DEPTH_TEST);

/* 4. ��ǰ��Ȳ��� */
// ���ڴ󲿷ֵ� GPU ���ṩһ��������ǰ��Ȳ���(Early Depth Testing)��Ӳ������
// ��ǰ��Ȳ���������Ȳ�����Ƭ����ɫ��֮ǰ����
// ֻҪ�������һ��Ƭ����Զ�����ǿɼ���(������������֮��), ���Ǿ�����ǰ�������Ƭ��
// Ƭ����ɫ��ͨ���������Ǻܴ��, ��������Ӧ�þ����ܱ�����������
// ��ʹ����ǰ��Ȳ���ʱ, Ƭ����ɫ����һ���������㲻��д��Ƭ�ε����ֵ
// ���һ��Ƭ����ɫ�����������ֵ������д��, ��ǰ��Ȳ����ǲ����ܵ�
// OpenGL ������ǰ֪�����ֵ

/* 5. ע����ʹ����Ȳ��Ե�ʱ����Ҫ���֮ǰ�����������Ϣ */
// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/* 6. ������Ȳ��Ե��ǲ����»��� */
// ��ĳЩ����������Ҫ������Ƭ�ζ�ִ����Ȳ��Բ�������Ӧ��Ƭ��, ����ϣ��������Ȼ���
// glDepthMask(GL_FALSE);

/* 7. ��Ȳ��Ժ��� */
// OpenGL ���������޸���Ȳ�����ʹ�õıȽ������
// ���������������� OpenGL ʲôʱ���ͨ������һ��Ƭ��, ʲôʱ��ȥ������Ȼ���
// ���ǿ��Ե��� glDepthFunc ���������ñȽ������(����˵��Ⱥ���(Depth Function))
// Ĭ��: glDepthFunc(GL_LESS),
// һЩ����������
// GL_ALWAYS     ��Զͨ����Ȳ���
// GL_NEVER      ��Զ��ͨ����Ȳ���
// GL_LESS       ��Ƭ�����ֵС�ڻ�������ֵʱͨ������
// GL_EQUAL      ��Ƭ�����ֵ���ڻ����������ֵʱͨ������
// GL_LEQUAL     ��Ƭ�����ֵС�ڵ��ڻ����������ֵʱͨ������
// GL_GREATER    ��Ƭ�����ֵ���ڻ����������ֵʱͨ������
// GL_NOTEQUAL   ��Ƭ�����ֵ�����ڻ����������ֵʱͨ������
// GL_GEQUAL     ��Ƭ�����ֵ���ڵ��ڻ����������ֵʱͨ������

/* 8. ���ֵ���� */
// [near, far] => [0, 1]
// ������Ȼ���: Linear Depth Buffer(ʵ������)
// ʵ���ϲ�����(���Լ� OpenGL ��ͶӰ������Ƶ�), ��ƽ�沿��ռ���˴󲿷ֵľ���
// http://www.songho.ca/opengl/gl_projectionmatrix.html
// https://banbao991.github.io/2021/01/31/CG/LS/05-1/
// ���ӻ�: gl_FragCoord.z �����������Ϣ


/* 9. ��ȳ�ͻ z-fighting */
// һ���ܳ������Ӿ������������ƽ����������ηǳ����ܵ�ƽ��������һ��ʱ�ᷢ��
// ��Ȼ���û���㹻�ľ���������������״�ĸ���ǰ��
// ���������������״���ϵ����л�ǰ��˳��, ��ᵼ�º���ֵĻ���
// ������������ȳ�ͻ (Z-fighting),
// ��Ϊ��������������������״������(Fight)˭�ô��ڶ���

// ��ȳ�ͻ����Ȼ����һ����������, ��������Զ��ʱЧ���������,
// ��Ϊ��Ȼ�����zֵ�Ƚϴ��ʱ�����Ÿ�С�ľ���

// ��ֹ��ȳ�ͻ�ķ���
// (1) ��Զ��Ҫ�Ѷ������ڵ�̫����, ���������ǵ�һЩ�����λ��ص�(΢Сƫ��)
//     ����������֮������һ���û��޷�ע�⵽��ƫ��ֵ, �������ȫ��������������֮�����ȳ�ͻ
// (2) �����ܽ���ƽ������ԶһЩ
//     ��ǰ�������ᵽ�˾����ڿ�����ƽ��ʱ�Ƿǳ��ߵ�, ����������ǽ���ƽ��Զ��۲���
//     ���ǽ��������ƽ��ͷ�����Ÿ���ľ���
//     Ȼ��, ����ƽ������̫Զ���ᵼ�½��������屻�ü���
//     ������ͨ����Ҫʵ���΢�����������ʺ���ĳ����Ľ�ƽ����롣
// (3) ʹ�ø��߾��ȵ���Ȼ���
//     �󲿷���Ȼ���ľ��ȶ���24λ��, �����ڴ󲿷ֵ��Կ���֧��32λ����Ȼ���
//     �⽫�Ἣ�����߾���, ����, ������һЩ����, ����ܻ�ø��߾��ȵ���Ȳ���, ������ȳ�ͻ


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    // ���Զ���Ȳ��Եĺ��������޸Ĳ鿴Ч��
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // Ĭ����Ϊ
    // glDepthFunc(GL_ALWAYS); // Ч����û������Ȳ���һ�� glDisable(GL_DEPTH_TEST)

    // build and compile shaders
    // -------------------------
    Shader shader("1.1.vs", "1.1.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // texture Coords
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
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture("marble.jpg");
    unsigned int floorTexture = loadTexture("metal.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}