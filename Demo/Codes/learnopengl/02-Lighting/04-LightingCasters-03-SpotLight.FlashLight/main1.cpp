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

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Light {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // attenuation
    float constant;
    float linera;
    float quadratic;

    // spot light
    float cutOff;
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void readMaterialAndLight(const char *materialPath, const char *lightPath);
unsigned int loadTexture(char const * path);

unsigned int SCR_WIDTH = 800U;
unsigned int SCR_HEIGHT = 800U;

// vertex shader, fragment shader
std::string VERTEX_SHADER("01.vert");
std::string FRAGMENT_SHADER("01.frag");

namespace Camera {
    float theta = 0.0f, phi = 0.0f;
    const float DELTA = 1.0f;
    float zAngle = 0.0f;
    float fov = 45.0f;
}

const float LIGHT_DELTA = 0.01f;
Light light;
Material material;

/*
 * �۹�
 * 
 * LightDir: ��Ƭ��ָ���Դ������
 * SpotDir: �۹���ָ��ķ���
 * Phi: ָ���˾۹�뾶���й��, ��������Ƕ�֮������嶼���ᱻ����۹�������
 * Theta: LightDir ������ SpotDir ����֮��ļн�(�ھ۹��ڲ��Ļ� Theta ֵӦ�ñ� Phi ֵС)
*/
int main(int argc, char *argv[]) {
    std::cout << "Move the light with x(a,z), y(s,x), z(d,c)!" << std::endl;
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
    glfwSetScrollCallback(window, scrollCallback);
    // glfwSetKeyCallback(window, keyCallback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader shaders(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    Shader lightingShader("light.vert", "light.frag");
    // �򵥵��������м��뷨����
    // �о��ֵֹ�,ͬһ�����в�ͬ�ķ���, ��ʵ�����, ���ڷ����ֵ
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    // ���ӵ�λ��
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    // texture coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
    // light source
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // ֻ��Ҫ�� VBO �����ٴ����� VBO ������
    // ��Ϊ���ӵ�VBO�������Ѿ���������ȷ�������嶥������
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // ���õ�������Ķ�������(�����ǵĵ���˵����ֻ��λ������)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ���� z-buffer ��Ȳ���
    glEnable(GL_DEPTH_TEST);
    // �������������� 
    unsigned int diffuseMap = loadTexture("container2.png");
    // �߹�����
    unsigned int specularMap = loadTexture("container2_specular.png");
    // �Է�������
    // unsigned int emissionMap = loadTexture("matrix.jpg");

    light.position = glm::vec3(0.8f, 0.8f, 0.8f);
    //light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    // readMaterialAndLight("material.txt", "light.txt");
    while (!glfwWindowShouldClose(window)) {
        // �����һ�ֱ��е������Ϣ
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // ��������Ϣ
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render starts
        shaders.use();
        
        // view, projection
        glm::mat4 view = glm::mat4(1.0f);
        glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 3.0f);
        view = glm::translate(view, -viewPos);
        view = glm::rotate(view, glm::radians(Camera::zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::rotate(view, glm::radians(Camera::theta), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, glm::radians(Camera::phi), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(
            glm::radians(Camera::fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaders.setTransformMatrix("view", view);
        shaders.setTransformMatrix("projection", projection);

        // light
        // ע���ԴҲҪ�任���۲�����ϵ��
        //glm::vec4 lightPos4(light.position, 1.0f);
        //lightPos4 = view * lightPos4;
        //shaders.setVec3("light.position", glm::vec3(
        //    lightPos4.x / lightPos4.w,
        //    lightPos4.y / lightPos4.w,
        //    lightPos4.z / lightPos4.w
        //));
        // ����Ϊ�ӵ�
        shaders.setVec3("light.position", 0.0f, 0.0f, 0.0f);
        // ����Ϊ�ӵ�ķ���(�ӵ�����ϵ), viewPos �����ӵ�����ϵ��
        glm::vec3 direction = -viewPos;
        shaders.setVec3("light.direction", direction);
        shaders.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        shaders.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        shaders.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // attenuation: 50, 1.0, 0.09, 0.032
        shaders.setFloat("light.constant", 1.0f);
        shaders.setFloat("light.linear", 0.09f);
        shaders.setFloat("light.quadratic", 0.032f);
        // spot light
        // ��һ������ֵ, ����Ƚ�
        shaders.setFloat("light.cutOff", glm::cos(glm::radians(7.5f)));

        // material
        shaders.setFloat("material.shininess", 64.0f);
        shaders.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        shaders.setInt("material.specular", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // model * 10
        glm::mat4 model;
        for (int i = 0; i < 10; ++i) {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(i*20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            shaders.setMat4("model", model);
            // �ڹ۲�����ϵ�м������
            glm::mat4 matrixForNormal = glm::transpose(glm::inverse(view * model));
            shaders.setMat4("matrixForNormal", matrixForNormal);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // ���Ƶ��Դ
        //lightingShader.use();
        //lightingShader.setMat4("projection", projection);
        //lightingShader.setMat4("view", view);
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, light.position);
        //model = glm::scale(model, glm::vec3(0.1f));
        //lightingShader.setMat4("model", model);
        //glBindVertexArray(lightVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        // unbind
        glBindVertexArray(0);
        // render ends
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    shaders.clear();
    glfwTerminate();
    return 0;
}

unsigned int loadTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    // ͨ����
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }

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

void readMaterialAndLight(const char *materialPath, const char *lightPath) {
    std::ifstream materialFile;
    std::ifstream lightFile;
    // ensure ifstream objects can throw exceptions:
    materialFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    lightFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        materialFile.open(materialPath);
        lightFile.open(lightPath);
        // read start
        // material
        for (int i = 0; i < 3; ++i) { materialFile >> material.ambient[i]; }
        for (int i = 0; i < 3; ++i) { materialFile >> material.diffuse[i]; }
        for (int i = 0; i < 3; ++i) { materialFile >> material.specular[i]; }
        materialFile >> material.shininess;
        // light
        for (int i = 0; i < 3; ++i) { lightFile >> light.ambient[i]; }
        for (int i = 0; i < 3; ++i) { lightFile >> light.diffuse[i]; }
        for (int i = 0; i < 3; ++i) { lightFile >> light.specular[i]; }
        // read end
        // close file handlers
        materialFile.close();
        lightFile.close();
    } catch (std::ifstream::failure& e) {
        std::cout
            << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n"
            << "ERROR CODE: " << e.code() << "\n"
            << "WHAT: " << e.what()
            << std::endl;
    }
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // �������Ҽ������ƶ����
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
    // light
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        light.position.x += LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        light.position.x -= LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        light.position.y += LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        light.position.y -= LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        light.position.z += LIGHT_DELTA;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        light.position.z -= LIGHT_DELTA;
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

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