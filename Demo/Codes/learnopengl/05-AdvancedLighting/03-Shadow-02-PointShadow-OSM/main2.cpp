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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void renderCube();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool blinn = false;
bool blinnKeyPressed = false;
bool lightPause = true;
bool lightPauseKeyPressed = false;
int onlyAmbient = 0;
bool onlyAmbientKeyPressed = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// cube VAO, VBO
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;

// �ϸ��̳�����ѧ�������ʹ����Ӱӳ�似��������̬��Ӱ
// Ч������, ����ֻ�ʺ϶����, ��Ϊ��Ӱֻ���ڵ�һ�����Դ�����ɵ�
// ������Ҳ�ж�����Ӱӳ��, ���(��Ӱ)��ͼ�����Զ������ӽ�

// �������ǵĽ������ڸ��ַ������ɶ�̬��Ӱ
// ����������������ڵ��Դ, �������з����ϵ���Ӱ

// ����������������Ӱ, ��ȥ��������������Ӱ��ͼ(omnidirectional shadow maps)����

// �������������������ͼ

// Ϊ����һ������Χ�����ֵ����������ͼ, ���Ǳ�����Ⱦ���� 6 ��, ÿ��һ����
// ��Ȼ��Ⱦ����6����Ҫ6����ͬ����ͼ����, ÿ�ΰ�һ����ͬ����������ͼ�渽�ӵ�֡���������

// ��Ⱦ����̫��!!!!!!

// Geometry Shader
// ���ܺķ�������Ϊһ�������ͼ����Ҫ���кܶ���Ⱦ����
// ����̳������ǽ�ת��ʹ�������һ��С�������������
// ������ɫ����������ʹ��һ����Ⱦ���������������������ͼ

int main(int argc, char** argv) {
    string shadowMapShaderSelVS("farAllSeeCube.vs"),
        shadowMapShaderSelFS("farAllSeeCube.fs");
    if (argc == 3) {
        shadowMapShaderSelVS = string(argv[1]);
        shadowMapShaderSelFS = string(argv[2]);
    }
    std::cout <<
        "Use: Demo.exe shadowMap.vs shadowMap.fs\n\n"
        "Press B to change Lighting Model(Phong/Blinn-Phong)\n"
        "Press Space to stop/start the light\n"
        "Press Q to on/off the diffuse and specular\n"
        << std::endl;
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE); // �������޳�, ֻ��Ҫ��һ����뼴��
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader shader(shadowMapShaderSelVS.c_str(), shadowMapShaderSelFS.c_str());
    Shader lightShader("light.vs", "light.fs");
    Shader depthShader("depthCube.vs", "depthCube.fs", "depthCube.gs");
    //Shader showDepthShader("showDepth.vs", "showDepth.fs");
    // ͸��ͶӰ
    //Shader showDepthShader("showDepthPerspective.vs", "showDepthPerspective.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // depth plane
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    const int cubesNumber = 4;
    glm::vec3 cubesTranslate[] = {
         glm::vec3(0.0f, 0.5f, 0.0f),
         glm::vec3(0.5f, 0.0f, 0.75f),
         glm::vec3(-0.25f, 0.0f, 0.5f),
         glm::vec3(4.0f, 4.0f, 4.0f)
    };
    float cubesScale[] = {
        0.25f, 0.25f, 0.125f, 0.25f
    };
    float cubeRotate[] = {
        0.0f, 30.0f, 60.0f, 45.0f
    };
    glm::vec3 cubeRotateAxis = glm::normalize(glm::vec3(1.0, 0.0, 1.0));

    // load textures
    // -------------
    unsigned int floorTexture = loadTexture("wood.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("floorTexture", 0);
    shader.setInt("depthMap", 1);

    // lighting info
    // -------------
    glm::vec3 lightPos(2.0f, 2.0f, 0.0f);
    float lightTime = 0.0f;

    // �����������ͼ
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMapFBO, depthCubeMap;
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthCubeMap);
    // ����֡��������һ�����ͼ
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap); // CUBE_MAP
    // �����ʽ����Ϊ���ͼ
    for (GLuint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // �󶨵�֡������
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    glDrawBuffer(GL_NONE); // ��ʽ���� OpenGL ���ǲ�ʹ���κ���ɫ���ݽ�����Ⱦ
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // update lightinh position
        if (!lightPause) {
            lightTime += 0.01f;
            lightPos.x = 2.0f*cos(lightTime);
            lightPos.z = 2.0f*sin(lightTime);
        }

        // input
        // -----
        processInput(window);

        // render
        // ------
        // (1) ��һ���л�����Դλ����Ⱦһ��õ����ͼ
        // ����׼��
        // 6 �������ͶӰ����
        GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
        GLfloat near = 1.0f;
        GLfloat far = 50.0f; // TODO Զ���ü��������Ҫ����
        // 90 �����ǲ��ܱ�֤��Ұ�㹻�󵽿��Ժ��ʵ�������������ͼ��һ����
        // ��������ͼ�������涼�����������ڱ�Ե����
        // cao �������� ������
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
        std::vector<glm::mat4> shadowTransforms;
        // ÿ������˳��ע������������ͼ�ĵ�һ������
        // �ҡ����ϡ��¡�����Զ
        const glm::vec3 faces[] = {
            glm::vec3(1.0f,  0.0f,  0.0f),
            glm::vec3(-1.0f, 0.0f,  0.0f),
            glm::vec3(0.0f,  1.0f,  0.0f),
            glm::vec3(0.0f, -1.0f,  0.0f),
            glm::vec3(0.0f,  0.0f,  1.0f),
            glm::vec3(0.0f,  0.0f, -1.0f)
        };
        const glm::vec3 ups[] = {
            glm::vec3(0.0f, -1.0f,  0.0f),
            glm::vec3(0.0f, -1.0f,  0.0f),
            glm::vec3(0.0f,  0.0f,  1.0f),
            glm::vec3(0.0f,  0.0f, -1.0f),
            glm::vec3(0.0f, -1.0f,  0.0f),
            glm::vec3(0.0f, -1.0f,  0.0f)
        };
        for (int i = 0; i < 6; ++i) {
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + faces[i], ups[i]));
        }
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        for (GLuint i = 0; i < 6; ++i) {
            depthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        }
        depthShader.setFloat("far_plane", far);
        // depthShader.setFloat("near_plane", near);
        depthShader.setVec3("lightPos", lightPos);

        // ��һ����Ⱦ��ʼ
        // �ذ�Ļ��Ʋ����޳�������, ��Ȼ��û��
        // ��ʵ�� OK ��, ��Ϊ�ذ������Ѿ�û�ж�����(�޳��˵ذ�û��Ӱ���)
        // glCullFace(GL_BACK); // �޳�������(�����������)
        // floor
        glm::mat4 model;
        // model = glm::mat4(1.0f);
        // model = glm::scale(model, glm::vec3(10.0f));
        // depthShader.setMat4("model", model);
        // renderCube();

        glCullFace(GL_FRONT); // �޳�������
        // cubes
        for (int i = 0; i < cubesNumber; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubesTranslate[i]);
            model = glm::rotate(model, cubeRotate[i], cubeRotateAxis);
            model = glm::scale(model, glm::vec3(cubesScale[i]));
            depthShader.setMat4("model", model);
            renderCube();
        }
        glCullFace(GL_BACK);

        // ��һ����Ⱦ����
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //// չʾ���ͼ
        //showDepthShader.use();
        //showDepthShader.setFloat("near_plane", near_plane);
        //showDepthShader.setFloat("far_plane", far_plane);
        //glBindVertexArray(quadVAO);
        //// ������
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, depthMap); // ��Ⱦ�õ������ͼ
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // �������߶λ��Ʒ���
        //glBindVertexArray(0);


        // (2) �ڶ�����Ⱦ, ������֮ǰ��Ⱦ�õ������ͼ������Ӱ����
        // ֻ��Ҫʹ�ù�Դλ�ý��в�������
        // ����Զƽ��, �ж��Ƿ���Ҫ����Ϊû����Ӱ
        // draw objects
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(10.0f));
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        // set light uniforms
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setBool("blinn", blinn);
        shader.setBool("onlyAmbient", onlyAmbient);
        // lightSpaceMatrix
        shader.setFloat("far_plane", far);
        // ����Ҫ��, ֻ��Ҫ�����Դ�ľ��������(depthCube.gs)
        // floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
        glDisable(GL_CULL_FACE); // �ڱ���������Ⱦ
        shader.setBool("reverse_normals", true);
        renderCube();
        shader.setBool("reverse_normals", false);
        glEnable(GL_CULL_FACE);

        // draw cube
        for (int i = 0; i < cubesNumber; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubesTranslate[i]);
            model = glm::rotate(model, cubeRotate[i], cubeRotateAxis);
            model = glm::scale(model, glm::vec3(cubesScale[i]));
            shader.setMat4("model", model);
            renderCube();
        }

        // draw light
        // ����Ҫ���������Ӱ������(������)
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        lightShader.setMat4("model", model);
        renderCube();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glfwTerminate();
    return 0;
}


// renderCube() Renders a 1x1 3D cube in NDC.
void renderCube() {
    // Initialize (if necessary)
    if (cubeVAO == 0) {
        GLfloat vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // Render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
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

    // Blinn-Phong / Phong
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed) {
        blinn = !blinn;
        std::cout << "Changed to " << (blinn ? "Blinn-Phong" : "Phong") << std::endl;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
        blinnKeyPressed = false;
    }

    // light moving or not
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !lightPauseKeyPressed) {
        lightPause = !lightPause;
        std::cout << "Light " << (lightPause ? "Stop" : "Start") << std::endl;
        lightPauseKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        lightPauseKeyPressed = false;
    }

    // diffuse and specular
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !onlyAmbientKeyPressed) {
        onlyAmbient = !onlyAmbient;
        std::cout << "Diffuse and Specular: " << (onlyAmbient ? "Off" : "On") << std::endl;
        onlyAmbientKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
        onlyAmbientKeyPressed = false;
    }
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
unsigned int loadTexture(char const * path) {
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}