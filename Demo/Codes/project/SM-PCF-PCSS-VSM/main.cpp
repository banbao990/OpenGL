#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <myUtils/basicModel.h>

#include <iostream>
#include <random>

#include "global.h"

using namespace BASIC_MODEL;

// DEBUG
//#define DEBUG_BANBAO

#define FPS_OUTPUT_FRAME 10

// 通用函数
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float currentFrame);
void checkError();

// 特有函数
void renderMainScene(Shader &shader);
void printInfos();

int main() {
    printInfos();
    // 窗口配置信息
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shadow Algorithm", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 禁止修改大小
    //glfwSetFramebufferSizeCallback(window, framebufferSizeCallback); 
    glfwSetWindowSizeLimits(window, SCR_WIDTH, SCR_HEIGHT, SCR_WIDTH, SCR_HEIGHT);
    
    // 捕捉鼠标
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 生成 DepthBuffer, 存储 Shadow Map
    // 为了不和之前的代码冲突, 我们额外创建一个帧缓冲用于存储 d/d^2

    //-------------------------------------------------------------- pass 1

    // 深度图 // 缺了无法开启深度测试(神奇的漏光现象)
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    // 纹理格式设置为深度图
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 绑定到帧缓冲中
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 如上我们不需要颜色缓冲
    // 然而, 不包含颜色缓冲的帧缓冲对象是不完整的
    // 所以我们需要显式告诉 OpenGL 我们不使用任何颜色数据进行渲染
    // 我们通过将调用 glDrawBuffer 和 glReadBuffer 把读和绘制缓冲设置为 GL_NONE 来做这件事
    // 深度的方差和均值(不需要 z-buffer)
    GLuint d_d2_Map;
    glGenTextures(1, &d_d2_Map);
    glBindTexture(GL_TEXTURE_2D, d_d2_Map);
    // 纹理格式设置为颜色缓冲
    // 2 通道
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 绑定到 DepthMap 中
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_d2_Map, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //-------------------------------------------------------------- pass 2&3
    GLuint d_d2_filter_FBO[2];
    glGenFramebuffers(2, d_d2_filter_FBO);
    GLuint d_d2_filter[2];
    glGenTextures(2, d_d2_filter);


#ifdef DEBUG_BANBAO
    std::cout << "C1: " << std::endl;
#endif
    checkError();

    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, d_d2_filter[i]);
        // 纹理格式设置为颜色缓冲
        // 2 通道
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // 绑定到 Frame Buffer 中
        glBindFramebuffer(GL_FRAMEBUFFER, d_d2_filter_FBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_d2_filter[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
#ifdef DEBUG_BANBAO
        std::cout << "C2: " << i << std::endl;
#endif
        checkError();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 加载 shader
    Shader blinnPhongShader("shaders/BlinnPhong.vert", "shaders/BlinnPhong_perspective.frag");
    Shader d_d2_Shader("shaders/gen_d_d2.vert", "shaders/gen_d_d2.frag");
    Shader SMShader("shaders/genSM.vert", "shaders/genSM_perspective.frag");
    Shader lightShader("shaders/light.vert", "shaders/light.frag");

    // 设置 shader 中的 uniform 参数
    blinnPhongShader.use();
    // 纹理
    blinnPhongShader.setInt("depthMap", 0);
    blinnPhongShader.setInt("d_d2_filter", 1);

    // OpenGL 状态
    glEnable(GL_DEPTH_TEST);

    // 开启面剔除会漏光, depthMap 的时候会有问题, 不过感觉最后一个 pass 的时候开启
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

#ifdef DEBUG_BANBAO
    std::cout << "C3: " << std::endl;
#endif
    checkError();
#ifdef DEBUG_BANBAO
    std::cout << "C4: " << std::endl;
#endif

    // 渲染
    while (!glfwWindowShouldClose(window)) {
        checkError();
        float currentFrame = (float)glfwGetTime();
        Global::deltaTime = currentFrame - Global::lastFrame;
        Global::lastFrame = currentFrame;
        Global::timeCost += Global::deltaTime;
        //if (++Global::frameCnt == FPS_OUTPUT_FRAME) {
        //    std::cout << "FPS: " << FPS_OUTPUT_FRAME / Global::timeCost << std::endl;
        //    Global::timeCost = 0;
        //    Global::frameCnt = 0;
        //}

        // 事件处理
        processInput(window, currentFrame);

        // MVP 矩阵
        glm::mat4 projection, view, model;

        // 小球运动参数
        if (Global::ballFirstRecord) {
            Global::ballFirstRecord = false;
            Global::ballStartTime = currentFrame;
        }
        float ballDeltaTime = 
            (currentFrame - Global::ballStartTime + Global::changeAddition) / 2.0f;
        ballDeltaTime -= int(ballDeltaTime);
        if (ballDeltaTime < 0.5f) {
            // h = 1.0 - t^2
            ballDeltaTime *= 2;
        } else {
            // h = (1.0 - t)^2
            ballDeltaTime = (1.0f - ballDeltaTime) * 2;
        }
        if (Global::ballBouncing) {
            Global::ballY = 1.0f - ballDeltaTime * ballDeltaTime;
        }

        //-------------------------------------------------------------- pass 1

        // 绑定输出到帧缓冲中
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // 清空 buffer
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f); // 注意这里的初始化, 1.0f 深度最大值
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // (1) 第一遍渲染获取到 shadow map
        // 透视投影
        projection = glm::perspective(
            glm::radians(60.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            Global::nearPlaneSM,
            Global::farPlaneSM
        );
        // 使用正交投影
        //projection = glm::ortho(-6.0f, 6.0f, -3.0f, 6.0f, Global::nearPlaneSM, Global::farPlaneSM);
        view = glm::lookAt(Global::lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

        // 光源为视点的观察坐标系
        glm::mat4 lightSpaceMatrix = projection * view;
        SMShader.use();
        SMShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // 渲染场景
        renderMainScene(SMShader);
        // 不需要渲染地板(最外层)
        model = glm::mat4(1.0f);
        model = glm::translate(model, Global::plane.translation);
        model = glm::scale(model, Global::plane.scale);
        SMShader.setMat4("model", model);
        SMShader.setVec3("color", Global::plane.color);
        renderPlane();

        //glfwSwapBuffers(window);
        //glfwPollEvents();
        //continue;

        //-------------------------------------------------------------- pass 2&3

        // 生成每一块周围的方差和均值
        // 对周围 (2R+1)^2 区域的值进行采样
        // 利用两趟 pass 优化

        // pass 2
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, d_d2_filter_FBO[0]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        d_d2_Shader.use();
        d_d2_Shader.setBool("vertical", false);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, d_d2_Map);

        renderQuad();

        //glfwSwapBuffers(window);
        //glfwPollEvents();
        //continue;

        // pass 3
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, d_d2_filter_FBO[1]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        d_d2_Shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, d_d2_filter[0]);

        d_d2_Shader.setBool("vertical", true);
        renderQuad();

        //glfwSwapBuffers(window);
        //glfwPollEvents();
        //continue;

        //-------------------------------------------------------------- pass 4

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // 清空 buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(
            glm::radians(Global::camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            Global::nearPlane,
            Global::farPlane
        );
        view = Global::camera.GetViewMatrix();

        // 设置 shader 参数
        blinnPhongShader.use();
        blinnPhongShader.setFloat("PCF_SampleRadius", Global::PCF_SampleRadius);
        blinnPhongShader.setFloat("SMDiffuse", Global::SMDiffuse);
        blinnPhongShader.setInt("SMType", Global::shadowType);
        blinnPhongShader.setFloat("lightWidth", Global::lightWidth);
        blinnPhongShader.setVec3("lightPos", Global::lightPosition);
        blinnPhongShader.setVec3("viewPos", Global::camera.Position);
        blinnPhongShader.setMat4("projection", projection);
        blinnPhongShader.setMat4("view", view);
        blinnPhongShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // 将深度图作为一张纹理贴图传进去
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap); // 原始非线性的深度图
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, d_d2_filter[1]);

        // 渲染主要场景
        renderMainScene(blinnPhongShader);

        // 渲染地板
        model = glm::mat4(1.0f);
        model = glm::translate(model, Global::plane.translation);
        model = glm::scale(model, Global::plane.scale);
        blinnPhongShader.setMat4("model", model);
        blinnPhongShader.setVec3("color", Global::plane.color);
        renderPlane();

        // 渲染光源
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, Global::lightPosition);
        model = glm::scale(model, glm::vec3(0.02f));
        lightShader.setMat4("model", model);
        renderCube();

        // 交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete
    // VAO, VBO, EBO
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &ballVAO);
    glDeleteBuffers(1, &ballVBO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    // texture
    glDeleteTextures(1, &depthMap);
    glDeleteTextures(1, &d_d2_Map);
    glDeleteTextures(2, d_d2_filter);
    // buffer
    glDeleteBuffers(1, &depthMapFBO);
    glDeleteBuffers(2, d_d2_filter_FBO);

    checkError();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// 渲染主要场景
void renderMainScene(Shader &shader) {
    // 渲染小球
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,
        Global::ball.translation
        + glm::vec3(0, Global::ballMaxHeight*Global::ballY, 0));
    model = glm::scale(model, Global::ball.scale);
    shader.setMat4("model", model);
    shader.setVec3("color", Global::ball.color);
    renderBall();

    // 渲染柱子
    //int columns = sizeof(Global::column) / sizeof(modelAttr);
    //for (int i = 0; i < columns; ++i) {
    //    model = glm::mat4(1.0f);
    //    model = glm::translate(model, Global::column[i].translation);
    //    model = glm::scale(model, Global::column[i].scale);
    //    shader.setMat4("model", model);
    //    shader.setVec3("color", Global::column[i].color);
    //    renderCube();
    //}
    for (int i = 0; i < 10; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.3f + 2.8f / 10 * i, 0, 0));
        model = glm::scale(model, Global::columnBase.scale);
        shader.setMat4("model", model);
        shader.setVec3("color", Global::columnBase.color);
        renderCube();
    }
}

// 输出提示信息
void printInfos() {
    std::cout <<
        "---------------------------------------------------------------------------------------------\n"
        "Use: Demo.exe\n\n"
        "Press W/A/S/D to change move the view point\n"
        "Press Space to toggle the ball' bounce!\n"
        "Press P/L to increase/decrease the width of the light!(Affect PCSS)\n"
        "Press I/J to increase/decrease the radius correction of the PCF sampling!(Affect PCF abd PCSS)\n"
        "Press O/K to increase/decrease the radius for block search!(Affect PCSS)\n"
        "Press 0/1/2/3/4 to choose the shadow type!\n"
        "    0(No Shadow), 1(Shadow Map), 2(PCF), 3(PCSS), 4(VSM)\n"
        "\n"
        "Default Parameters:\n"
        "    light width: " << Global::lightWidth << "\n"
        "    radius correction of the PCF sampling: " << Global::PCF_SampleRadius << "\n"
        "    radius correction for block search: " << Global::SMDiffuse << "\n"
        "    shadow type: " << Global::shadowType << ": " << Global::SMDisp[Global::shadowType] << "\n"
        "---------------------------------------------------------------------------------------------\n"
        << std::endl;
}

// 键盘事件
void processInput(GLFWwindow *window, float currentFrame) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Global::camera.ProcessKeyboard(FORWARD, Global::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Global::camera.ProcessKeyboard(BACKWARD, Global::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Global::camera.ProcessKeyboard(LEFT, Global::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Global::camera.ProcessKeyboard(RIGHT, Global::deltaTime);

    // 小球跳动
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !Global::spacePressed) {
        Global::spacePressed = true;
        std::cout << "Ball: " << (Global::ballBouncing ? "Stop" : "Start") << " bouncing!" << std::endl;
        // 停下来
        if (Global::ballBouncing) {
            Global::changeAddition += currentFrame;
        } else {
            Global::changeAddition -= currentFrame;
        }
        Global::ballBouncing = !Global::ballBouncing;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        Global::spacePressed = false;
    }

    // 模型上下移动
    //if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    //    Global::ball.translation.y += 0.1f;
    //    std::cout << "Ball Position(y):" << Global::ball.translation.y << std::endl;
    //}
    //if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    //    Global::ball.translation.y -= 0.1f;
    //    std::cout << "Ball Position(y):" << Global::ball.translation.y << std::endl;
    //}

    // 光源大小
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        Global::lightWidth += 0.001f;
        std::cout << "Light Width: " << Global::lightWidth << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        Global::lightWidth -= 0.001f;
        std::cout << "Light Width: " << Global::lightWidth << std::endl;
    }

    // 阴影的弥散参数
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        Global::SMDiffuse += 0.1f;
        std::cout << "Shadow Diffuse: " << Global::SMDiffuse << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        Global::SMDiffuse -= 0.1f;
        std::cout << "Shadow Diffuse: " << Global::SMDiffuse << std::endl;
    }

    // PCF 采样距离修正
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        Global::PCF_SampleRadius += 0.01f;
        std::cout << "PCF Sample Radius: " << Global::PCF_SampleRadius << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        Global::PCF_SampleRadius -= 0.01f;
        std::cout << "PCF Sample Radius: " << Global::PCF_SampleRadius << std::endl;
    }

    // 选择阴影类别
    // Pressed 保证只触发一次
    for (int i = 0; i <= SHADOW_KIND; ++i) {
        if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS
            && !Global::SMKindPressed[i]) {
            Global::SMKindPressed[i] = true;
            Global::shadowType = i;
            std::cout << "Use " << Global::SMDisp[i] << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_RELEASE) {
            Global::SMKindPressed[i] = false;
        }
    }
}

// 窗口变化事件
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 鼠标事件
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (Global::firstMouse) {
        Global::lastX = (float)xpos;
        Global::lastY = (float)ypos;
        Global::firstMouse = false;
    }

    float xoffset = (float)xpos - Global::lastX;
    // OpenGL 的 y 轴正向向下
    float yoffset = Global::lastY - (float)ypos;

    Global::lastX = (float)xpos;
    Global::lastY = (float)ypos;

    Global::camera.ProcessMouseMovement(xoffset, yoffset);
}

// 滚轮事件
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Global::camera.ProcessMouseScroll((float)yoffset);
}

// 检查错误
void checkError() {
    int num = 0;
    GLenum a = GL_NO_ERROR;
    while (true) {
        a = glGetError();
        if (a == GL_NO_ERROR) {
            break;
        }
        std::cout << " ERROR(" << +num << "): " << a << std::endl;
    }
}