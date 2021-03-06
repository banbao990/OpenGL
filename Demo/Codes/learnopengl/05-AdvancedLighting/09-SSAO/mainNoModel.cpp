#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <random>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderQuad();
void renderCube();
void renderBall(const int XSEG = 40, const int YSEG = 30, bool generateNormal = true);

// 窗口设定
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 相机 camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// 帧间时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// (1-f)a + fb
float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

// 模型的位置

float modelPos = 0.5f;
// 中间过程
const int totalProcess = 6;
int kind = totalProcess;
string kindDisp[] = {
    string("G-Buffer: Position(view coordinate)"),
    string("G-Buffer: Normal(view coordinate)"),
    string("G-Buffer: Diffuse(Albedo)"),
    string("SSAO: Ambient Occlusion"),
    string("SSAO: Ambient Occlusion(Blur)"),
    string("Final Scene")
};
bool kindShiftKeyPressed[totalProcess] = { false };

// SSAO
bool SSAOOn = true;
bool SSAOOnKeyPressed = false;

int main() {
    std::cout <<
        "Press O/P to move the model\n"
        "Press Space to on/off the SSAO\n"
        "Press 1/.../6 to see the middel process\n\n"
        << std::endl;
    // 窗口配置信息
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
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
    // 捕捉鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // OpenGL 状态
    glEnable(GL_DEPTH_TEST);

    // shaders
    Shader shaderGeometryPass("9.ssao_geometry.vs", "9.ssao_geometry.fs");
    Shader shaderLightingPass("9.ssao.vs", "9.ssao_lighting.fs");
    Shader shaderSSAO("9.ssao.vs", "9.ssao.fs");
    Shader shaderSSAOBlur("9.ssao.vs", "9.ssao_blur.fs");
    // 为了可视化
    Shader simpleShader("9.simple.vs", "9.simple.fs");

    // G-Buffer
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedo;
    // position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL); // 浮点帧
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 溢出取边缘
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // 显式绑定 3 个颜色缓冲
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // depth
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 方便循环(用于可视化)
    unsigned int GBuffersForCycle[] = { gPosition, gNormal, gAlbedo };

    // 需要一个帧缓冲需要存储 SSAO 阶段的结果
    // ssaoBlur 是将 SSAO 阶段得到的结果进行模糊化处理(分辨率?)
    unsigned int ssaoFBO, ssaoBlurFBO;
    // ssao 帧缓冲
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    // 由于环境遮蔽的结果是一个灰度值, 我们将只需要用纹理的一个分量保存即可 GL_RED
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    
    // blur 帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate sample kernel
    // ----------------------
    // 我们将在切线空间(Tangent Space)内生成采样核心
    // 法向: (0,0,1)
    // generates random floats between 0.0 and 1.0
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i) {
        // 生成的向量: z > 0
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample); // 单位化
        sample *= randomFloats(generator); // 长度均匀分布
        float scale = float(i) / 64.0;

        // scale samples s.t. they're more aligned to center of kernel
        // 我们更愿意将更多的注意放在靠近真正片段的遮蔽上, 也就是将核心样本靠近原点分布
        // 0.1*(1 - scale*scale) + 1.0*(scale*scale)
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale; // 缩小
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    // 通过引入一些随机性到采样核心上, 我们可以大大减少获得不错结果所需的样本数量
    // 我们可以对场景中每一个片段创建一个随机旋转向量, 但这会很快将内存耗尽
    // 所以, 更好的方法是创建一个小的随机旋转向量纹理平铺在屏幕上
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++) {
        // rotate around z-axis (in tangent space)
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
        ssaoNoise.push_back(noise);
    }
    // 包含随机旋转向量的4x4纹理
    // 设定它的封装方法为GL_REPEAT, 从而保证它合适地平铺在屏幕上
    unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 光源信息
    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

    // shader 配置信息
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedo", 2);
    shaderLightingPass.setInt("ssao", 3);
    shaderSSAO.use();
    shaderSSAO.setInt("gPosition", 0);
    shaderSSAO.setInt("gNormal", 1);
    shaderSSAO.setInt("texNoise", 2);
    shaderSSAOBlur.use();
    shaderSSAOBlur.setInt("ssaoInput", 0);
    simpleShader.use();
    simpleShader.setInt("scene", 0);

    // 渲染
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 事件处理
        processInput(window);

        // 渲染
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        simpleShader.use();
        simpleShader.setBool("useR", false);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        // G-Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        // room cube
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
        model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
        shaderGeometryPass.setMat4("model", model);
        shaderGeometryPass.setInt("invertedNormals", 1); // invert normals as we're inside the cube
        renderCube();
        shaderGeometryPass.setInt("invertedNormals", 0);
        // backpack model on the floor
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, modelPos, 0.0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.2f));
        shaderGeometryPass.setMat4("model", model);
        //backpack.Draw(shaderGeometryPass);
        renderBall();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (kind == 1 || kind == 2 || kind == 3) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            simpleShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, GBuffersForCycle[kind - 1]);
            renderQuad();
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // 2. generate SSAO texture
        // ------------------------
        // 生成 SSAO 遮蔽值
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.use();
        // Send kernel + rotation 
        for (unsigned int i = 0; i < 64; ++i)
            shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        shaderSSAO.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (kind == 4) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            simpleShader.use();
            simpleShader.setBool("useR", true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            renderQuad();
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        // 模糊效果
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (kind == 5) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            simpleShader.use();
            simpleShader.setBool("useR", true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            renderQuad();
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
        // -------------------------
        // 计算光照
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        // send light relevant uniforms
        glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
        shaderLightingPass.setVec3("light.Position", lightPosView);
        shaderLightingPass.setVec3("light.Color", lightColor);
        // Update attenuation parameters
        const float linear = 0.09;
        const float quadratic = 0.032;
        shaderLightingPass.setFloat("light.Linear", linear);
        shaderLightingPass.setFloat("light.Quadratic", quadratic);
        // SSAO on/off
        shaderLightingPass.setBool("SSAOOn", SSAOOn);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        renderQuad();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


// 渲染一个球体 R = 1
#define MY_PI 3.1415926f
unsigned int ballVAO = 0;
unsigned int ballVBO = 0;
unsigned int ballEBO = 0;
void renderBall(const int XSEG , const int YSEG , bool generateNormal) {
    const int totalNodes = 2 + (YSEG - 1)*XSEG;
    const int totalIndices = (YSEG - 1) * 2 * XSEG;
    // 第一次绘制, 生成数据并传输给 GPU
    if (ballVAO == 0) {
        // 生成球体数据, 类似于经纬线, 只需要位置和法向即可(位置和法向是相同的)
        int nodesTransfer = generateNormal ? totalNodes * 2 : totalNodes;
        glm::vec3* nodes = new glm::vec3[nodesTransfer];
        nodes[0].x = 0.0f;
        nodes[0].z = 0.0f;
        nodes[0].y = 1.0f;
        int num = 1;
        for (int i = 0; i < XSEG; ++i) {
            float x = cos(1.0*i / XSEG * 2 * MY_PI), y = sin(1.0*i / XSEG * 2 * MY_PI);
            float z, rate;
            for (int j = 1; j < YSEG; ++j) {
                z = (2.0f * j / YSEG - 1.0f);
                rate = sqrt(1 - z * z);
                nodes[num].x = x * rate;
                nodes[num].z = y * rate;
                nodes[num++].y = -z;
            }
        }
        nodes[num].x = 0.0f;
        nodes[num].z = 0.0f;
        nodes[num].y = -1.0f;
        std::cout << "Total nods in ball: " << totalNodes << " = " << ++num << std::endl;
        if (generateNormal) {
            // 法线数据(和顶点数据一致)
            memcpy(nodes + totalNodes, nodes, sizeof(nodes[0])*totalNodes);
        }
        // 生成引用数据
        unsigned int* indices = new unsigned int[totalIndices * 3];
        num = 0;
        const int YSEGMINUS1 = YSEG - 1;
        // 注意三角形逆时针
        for (int i = 0; i < XSEG; ++i) {
            int ii = (i + 1) % XSEG;
            int i_YSEGMINUS1 = i * YSEGMINUS1;
            int ii_YSEGMINUS1 = ii * YSEGMINUS1;
            indices[num++] = 0;
            indices[num++] = i_YSEGMINUS1 + 1;
            indices[num++] = ii_YSEGMINUS1 + 1;
            for (int j = 1; j < YSEGMINUS1; ++j) {
                indices[num++] = i_YSEGMINUS1 + j;
                indices[num++] = i_YSEGMINUS1 + j + 1;
                indices[num++] = ii_YSEGMINUS1 + j;
                indices[num++] = i_YSEGMINUS1 + j + 1;
                indices[num++] = ii_YSEGMINUS1 + j + 1;
                indices[num++] = ii_YSEGMINUS1 + j;
            }
            indices[num++] = totalNodes - 1;
            indices[num++] = ii_YSEGMINUS1 + YSEGMINUS1;
            indices[num++] = i_YSEGMINUS1 + YSEGMINUS1;
        }
        std::cout << "Total nodes in ball:(3*)" << totalIndices * 3 << " = " << num << std::endl;
        glGenVertexArrays(1, &ballVAO);
        glGenBuffers(1, &ballVBO);
        glGenBuffers(1, &ballEBO);
        glBindVertexArray(ballVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(nodes[0]) * nodesTransfer, nodes, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ballEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*totalIndices * 3, indices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        if (generateNormal) {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(nodes[0]) * totalNodes));
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        // EBO 的绑定信息保存在 VAO 之中
        // 如果在 VAO 还没解绑的时候重新绑定, 就修改了 VAO 中保存的 EBO 索引, 这样就出大问题
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        delete[] nodes;
        delete[] indices;
    }
    glBindVertexArray(ballVAO);
    glDrawElements(GL_TRIANGLES, totalIndices * 3, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

// 渲染一个立方体 2*2*2
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube() {
    // initialize (if necessary)
    if (cubeVAO == 0) {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


// 渲染一个平面 2*2
unsigned int quadVAO = 0;
unsigned int quadVBO;
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
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// 键盘事件
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

    // SSAO
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !SSAOOnKeyPressed) {
        SSAOOn = !SSAOOn;
        std::cout << "SSAO: " << (SSAOOn ? "on" : "off") << std::endl;
        SSAOOnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        SSAOOnKeyPressed = false;
    }

    // 中间过程的切换
    for (int i = 0; i < totalProcess; ++i) {
        if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_PRESS && !kindShiftKeyPressed[i]) {
            kind = i + 1;
            std::cout << "See the middle process: " << kindDisp[i] << std::endl;
            kindShiftKeyPressed[i] = true;
        }
        if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_RELEASE) {
            kindShiftKeyPressed[i] = false;
        }
    }

    // 模型上下位置
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        modelPos -= 0.01f;
        std::cout << "Model Position(y): " << modelPos << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        modelPos += 0.01f;
        std::cout << "Model Position(y): " << modelPos << std::endl;
    }
}

// 窗口变化事件
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 鼠标事件
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

// 滚轮事件
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}