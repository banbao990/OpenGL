#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <learnopengl/camera.h>

#define SHADOW_KIND 4

#ifndef SCR_WIDTH
    // 窗口设定
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#endif

struct modelAttr {
    glm::vec3 scale, translation, color;
    modelAttr(glm::vec3 s, glm::vec3 t, glm::vec3 c)
        : scale(s), translation(t), color(c) {}
};

// 一些全局变量
namespace Global {
    //----------------------- DEBUG


    //----------------------- 全局参数
    // SM 远近裁剪面
    const float nearPlaneSM = 2.0f;
    const float farPlaneSM = 20.0f;

    // 正常视点远近裁剪面
    const float nearPlane = 0.1f;
    const float farPlane = 50.0f;



    //----------------------- 光源
    glm::vec3 lightPosition(0.0f, 3.0f, -3.0f);
    float lightWidth = 0.132f;



    //----------------------- 小球模型
    modelAttr ball(
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.0f, -0.6f, 1.0f),
        glm::vec3(0.5f, 0, 0)
    );

    // 小球运动参数
    float ballMaxHeight = 1.8f;
    float ballStartTime;
    bool ballFirstRecord = true;
    float ballY = 1.0f;
    // 小球跳动
    bool spacePressed = false;
    bool ballBouncing = true;
    float changeAddition = 0;

    //----------------------- 柱子模型
    modelAttr column[2] = {
        modelAttr(
            glm::vec3(0.05f, 0.7f, 0.05f),
            glm::vec3(1.0f, -0.35f, 0.0f),
            glm::vec3(0, 0, 0.5f)
        ),
        modelAttr(
            glm::vec3(0.05f, 1.0f, 0.05f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.13f, 0.53f, 1.0f)
        ),
    };

    modelAttr columnBase(
        glm::vec3(0.05f, 1.0f, 0.05f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.13f, 0.53f, 1.0f)
    );

    //----------------------- 地板模型
    modelAttr plane(
        glm::vec3(3.0f, 1.0f, 5.0f),
        glm::vec3(0, -1.001f, 4.0f),
        glm::vec3(0.3f, 0.3f, 0.3f)
    );


    //----------------------- 阴影
    bool SMKindPressed[SHADOW_KIND + 1] = {};
    std::string SMDisp[SHADOW_KIND + 1] = {
        std::string("No Shadow"),
        std::string("SM(Shadow Mapping)"),
        std::string("PCF(Percentage Closer Filtering)"),
        std::string("PCSS(Percentage Closer Soft Shadows)"),
        std::string("VSM(Variance Shadow Mapping)"),
    };
    int shadowType = 4;
    // 阴影的弥散参数
    float SMDiffuse = 19.0f;
    // 阴影 PCF 的采样宽度
    float PCF_SampleRadius = 0.588f;



    //----------------------- 相机控制
    Camera camera(glm::vec3(0.0f, 0.5f, 10.0f));
    float lastX = (float)SCR_WIDTH / 2.0;
    float lastY = (float)SCR_HEIGHT / 2.0;
    bool firstMouse = true;



    //----------------------- 帧时间控制
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    // FPS 输出
    int frameCnt = 0;
    float timeCost = 0;
}
