#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <random>
using namespace std;

#define MY_PI 3.1415926f

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderQuad();
void renderCube();
void renderPlane();
GLuint generateRandomTexture();

// settings
const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 显示中间过程
int quadChange = 0;
bool quadChangeKeyPressed = false;
bool RSMOn = true;
bool RSMOnKeyPressed = false;

// 参数
int randomSamples = 4000;
float radius = 0.9f; // 采样半径
float indirectStrength = 1.0f; // 间接光照强度

// VAO, VBO
GLuint planeVAO = 0;
GLuint planeVBO = 0;
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
GLuint quadVAO = 0;
GLuint quadVBO = 0;

int main(int argc, char** argv) {
    // print infos
    cout <<
        "Use: Demo.exe (random samples)\nex:Demo.exe " << randomSamples << "\n\n"
        "Press W/A/S/D to change move the view point\n"
        "Press Space to see the middle process\n"
        "Press O/P to increase/decrease the max-radius for sampling\n"
        "Press K/L to increase/decrease the strength of indirect lighting\n"
        "Press R to on/off the RSM\n"
        "\n"
        "default parameters:\n"
        "    max-radius: " << radius << "\n"
        "    indirect light strength: " << indirectStrength << "\n"
        "    samples: " << randomSamples << "\n"
        << endl;
    if (argc == 2) {
        randomSamples = atoi(argv[1]);
    }
    // glfw 窗口管理配置信息
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Reflective Shadow Map", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL 全局状态
    glEnable(GL_DEPTH_TEST);

    // shaders 加载
    // 第一遍渲染
    Shader sceneShader("shaders/scene.vs", "shaders/scene.fs");
    //Shader sceneShader("shaders/sceneSimple.vs", "shaders/sceneSimple.fs");
    // 第二遍渲染
    Shader rsmShader("shaders/rsm.vs", "shaders/rsm.fs");
    // 渲染光源
    Shader lightShader("shaders/light.vs", "shaders/light.fs");

    // 用于显示中间过程
    Shader showBufferShader("shaders/show.vs", "shaders/show.fs");

    // 模型信息
    // 向墙角平移
    const float cornerOff = 0.0f;
    // 三个平面
    glm::mat4 planeModel = glm::mat4(1.0f);
    // 三个小立方体
    glm::mat4 models[] = {
        glm::mat4(1.0f),
        glm::mat4(1.0f),
        glm::mat4(1.0f)
    };
    // 0.699 避免重合
    models[0] = glm::translate(models[0], glm::vec3(0.0f + cornerOff, -0.699f, 0.0f - cornerOff));
    models[0] = glm::rotate(models[0], -(float)atan(1), glm::vec3(0.0f, 1.0f, 0.0f));
    models[0] = glm::scale(models[0], glm::vec3(0.3f));

    models[1] = glm::translate(models[1], glm::vec3(0.0f + cornerOff, -0.198f, 0.0f - cornerOff));
    models[1] = glm::rotate(models[1], -(float)atan(1), glm::vec3(0.0f, 1.0f, 0.0f));
    models[1] = glm::scale(models[1], glm::vec3(0.2f));

    models[2] = glm::translate(models[2],
        glm::vec3(0.0f + cornerOff, 0.1f*((float)sqrt(3)), 0.0f - cornerOff));
    models[2] = glm::rotate(models[2],
        -(float)atan(sqrt(2)), glm::vec3(1.0f, 0.0f, -1.0f));
    models[2] = glm::scale(models[2], glm::vec3(0.1f));

    // 帧缓冲
    // 这里的 G-Buffer 并不是真正的 G-Buffer, 保存的是以光源为视点的帧缓冲
    GLuint gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint gBuffers[4];
    // 世界坐标系位置缓冲、世界坐标系法向缓冲、光通量缓冲
    glGenTextures(3, gBuffers);
    for (int i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, gBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, gBuffers[i], 0);
    }
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // 深度缓冲
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    gBuffers[3] = depthMap;
    glBindTexture(GL_TEXTURE_2D, depthMap);
    // 纹理格式设置为深度图
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // 检查帧缓冲是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cout << "Framebuffer not complete!" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 采样纹理贴图
    GLuint randomTexture = generateRandomTexture();

    // 光源信息
    glm::vec3 lightPos = glm::vec3(-3.0f, 4.0f, 5.0f);

    // shader 常量设置
    // 光源为视点的变换矩阵
    sceneShader.use();
    const float nearPlane = 0.1f, farPlane = 20.0f;
    glm::mat4 lightProjection, lightView, lightSpaceMatrix;
    lightProjection = glm::perspective(45.0f, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlane, farPlane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    sceneShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    sceneShader.setVec3("lightPos", lightPos);

    // 设定 G-Buffer 纹理内容
    rsmShader.use();
    rsmShader.setInt("gPosition", 0);
    rsmShader.setInt("gNormal", 1);
    rsmShader.setInt("gFlux", 2);
    rsmShader.setInt("gDepthMap", 3);
    rsmShader.setInt("gRandomMap", 4);
    rsmShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    rsmShader.setVec3("lightPos", lightPos);
    rsmShader.setInt("randomSamples", randomSamples);

    // 显示纹理
    showBufferShader.use();
    showBufferShader.setInt("scene", 0);
    showBufferShader.setFloat("farPlane", farPlane);
    showBufferShader.setFloat("nearPlane", nearPlane);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 时间(用于鼠标、滚轮事件)
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入事件
        processInput(window);

        // 开始渲染图形
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 投影矩阵、观察矩阵
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearPlane, farPlane);
        glm::mat4 view = camera.GetViewMatrix();

        sceneShader.use();
        //sceneShader.setMat4("projection", projection);
        //sceneShader.setMat4("view", view);

        // 第一次渲染
        // 得到深度图、世界坐标系位置图、世界坐标系法向图、光通量图
        // 后面三张图的位置是深度图对应的最近的点
        // 需要切换到以光源为视点的观察坐标系计算阴影
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        // 和下面一行代码顺序不能反
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sceneShader.use();
        // 顺序影响性能, 深度测试结果
        for (int i = 0; i < 3; ++i) {
            sceneShader.setMat4("model", models[i]);
            renderCube();
        };
        sceneShader.setMat4("model", planeModel);
        renderPlane();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 第二次渲染
        // 还是得跟第一遍一样绘制整个场景
        // 计算直接光照明以及 shadow map 中记录的点的间接光照
        // 直接利用 G-Buffer 进行计算

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rsmShader.use();
        // 设置变换矩阵
        rsmShader.setMat4("projection", projection);
        rsmShader.setMat4("view", view);
        rsmShader.setVec3("viewPos", camera.Position);
        // TODO rsm on/off
        rsmShader.setBool("rsmOn", RSMOn);
        rsmShader.setFloat("radius", radius);
        rsmShader.setFloat("indirectStrength", indirectStrength);
        // 纹理
        for (int i = 0; i < 4; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, gBuffers[i]);
        }
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, randomTexture);
        // 开始渲染场景中的物体
        for (int i = 0; i < 3; ++i) {
            rsmShader.setMat4("model", models[i]);
            renderCube();
        };
        rsmShader.setMat4("model", planeModel);
        renderPlane();

        // 渲染光源
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        lightShader.setMat4("model", model);
        renderCube();

        // 交换缓冲到窗口
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // free
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

/*
        // 展示中间过程
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        showBufferShader.use();
        showBufferShader.setBool("isDepth", quadChange == 3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffers[quadChange]);
        renderQuad();
        glfwSwapBuffers(window);
        glfwPollEvents();
        continue;
*/


// 生成一个随机采样贴图
GLuint generateRandomTexture() {
    // sizeof(glm::vec3) = 12, 在内存中连续存放, 恰好为 float * 3
    glm::vec3 *randomMap = new glm::vec3[randomSamples];
    uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    default_random_engine generator;
    for (int i = 0; i < randomSamples; ++i) {
        float k1 = randomFloats(generator), k2 = randomFloats(generator);
        // (x, y) 为位置, z 为权重
        randomMap[i].x = k1 * sin(2 * MY_PI * k2);
        randomMap[i].y = k1 * cos(2 * MY_PI * k2);
        randomMap[i].z = k1 * k1;
    }
    // 生成纹理
    // 注意只需要 1 维纹理即可(1D 纹理采样不知道为什么间接光照一直都是0)
    //GLuint randomTexture;
    //glGenTextures(1, &randomTexture);
    //glBindTexture(GL_TEXTURE_1D, randomTexture);
    //glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, randomSamples, 0, GL_RGB, GL_FLOAT, randomMap);
    // 不需要设定参数, 我们的采样不会溢出
    GLuint randomTexture;
    glGenTextures(1, &randomTexture);
    glBindTexture(GL_TEXTURE_2D, randomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, randomSamples, 1, 0, GL_RGB, GL_FLOAT, randomMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    delete[] randomMap;
    return randomTexture;
}

// 注意这里的 plane 和 cube 的数据布局是一致的, 因此可以使用同一个 shader
// 渲染 3 个小平面(后右下)(原始大小 2*2)
void renderPlane() {
    // initialize (if necessary)
    if (planeVAO == 0) {
        // 注意这里的法向需要修改为相反的(已修改)
        // 相当于从内向外渲染
        float vertices[] = {
            // position           // normal        // color
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.4f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.4f, 0.0f, 0.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.4f, 0.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.4f, 0.0f, 0.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.4f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.4f, 0.0f, 0.0f, // top-left
            // right face
             1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.0f, // top-right
             1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.4f,// top-right
             1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.4f,// top-left
             1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.4f,// bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.4f,// bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.4f,// bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.4f,// top-right
        };
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(planeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glBindVertexArray(0);
}

// 渲染小立方体(原始大小 2*2*2)
void renderCube() {
    // initialize (if necessary)
    if (cubeVAO == 0) {
        float vertices[] = {
            // position           // normal           // color
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.7f, 0.7f, 0.7f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.7f, 0.7f, 0.7f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.7f, 0.7f, 0.7f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.7f, 0.7f, 0.7f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.7f, 0.7f, 0.7f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.7f, 0.7f, 0.7f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.7f, 0.7f, 0.7f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.7f, 0.7f, 0.7f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.7f, 0.7f, 0.7f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.7f, 0.7f, 0.7f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.7f, 0.7f, 0.7f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.7f, 0.7f, 0.7f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-right
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.7f, 0.7f, 0.7f, // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


// 渲染窗口(原始大小 2*2)
// 用于将渲染得到的纹理图展示出来
void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// 处理键盘输入
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

    // 是否开启 RSM
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !RSMOnKeyPressed) {
        RSMOn = !RSMOn;
        cout << "RSM: " << (RSMOn ? "On" : "Off") << endl;
        RSMOnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
        RSMOnKeyPressed = false;
    }

    // 切换场景(表现中间过程)
    //if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !quadChangeKeyPressed) {
    //    quadChange += 1;
    //    if (quadChange > 3) {
    //        quadChange = 0;
    //    }
    //    cout << quadChange << endl;
    //    quadChangeKeyPressed = true;
    //}
    //if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
    //    quadChangeKeyPressed = false;
    //}

    // 切换采样半径
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        radius += 0.01f;
        cout << "Max sample radius: " << radius << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        radius -= 0.01f;
        cout << "Max sample radius: " << radius << endl;
    }

    // 切换间接光照强度
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        indirectStrength += 0.01f;
        cout << "Indrect Light Strength: " << indirectStrength << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        indirectStrength -= 0.01f;
        if (indirectStrength < 0.0f) {
            indirectStrength = 0.0f;
        }
        cout << "Indrect Light Strength: " << indirectStrength << endl;
    }
}

// 处理屏幕大小变化
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 处理鼠标变化事件
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = ((float)xpos) - lastX;
    // 注意 OpenGL y 的正方向是朝上的
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 处理滚轮事件
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll((float)yoffset);
}