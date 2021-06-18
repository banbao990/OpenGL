#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <learnopengl/camera.h>

#define SHADOW_KIND 4

#ifndef SCR_WIDTH
    // �����趨
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#endif

struct modelAttr {
    glm::vec3 scale, translation, color;
    modelAttr(glm::vec3 s, glm::vec3 t, glm::vec3 c)
        : scale(s), translation(t), color(c) {}
};

// һЩȫ�ֱ���
namespace Global {
    //----------------------- DEBUG


    //----------------------- ȫ�ֲ���
    // SM Զ���ü���
    const float nearPlaneSM = 2.0f;
    const float farPlaneSM = 20.0f;

    // �����ӵ�Զ���ü���
    const float nearPlane = 0.1f;
    const float farPlane = 50.0f;



    //----------------------- ��Դ
    glm::vec3 lightPosition(0.0f, 3.0f, -3.0f);
    float lightWidth = 0.132f;



    //----------------------- С��ģ��
    modelAttr ball(
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.0f, -0.6f, 1.0f),
        glm::vec3(0.5f, 0, 0)
    );

    // С���˶�����
    float ballMaxHeight = 1.8f;
    float ballStartTime;
    bool ballFirstRecord = true;
    float ballY = 1.0f;
    // С������
    bool spacePressed = false;
    bool ballBouncing = true;
    float changeAddition = 0;

    //----------------------- ����ģ��
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

    //----------------------- �ذ�ģ��
    modelAttr plane(
        glm::vec3(3.0f, 1.0f, 5.0f),
        glm::vec3(0, -1.001f, 4.0f),
        glm::vec3(0.3f, 0.3f, 0.3f)
    );


    //----------------------- ��Ӱ
    bool SMKindPressed[SHADOW_KIND + 1] = {};
    std::string SMDisp[SHADOW_KIND + 1] = {
        std::string("No Shadow"),
        std::string("SM(Shadow Mapping)"),
        std::string("PCF(Percentage Closer Filtering)"),
        std::string("PCSS(Percentage Closer Soft Shadows)"),
        std::string("VSM(Variance Shadow Mapping)"),
    };
    int shadowType = 4;
    // ��Ӱ����ɢ����
    float SMDiffuse = 19.0f;
    // ��Ӱ PCF �Ĳ������
    float PCF_SampleRadius = 0.588f;



    //----------------------- �������
    Camera camera(glm::vec3(0.0f, 0.5f, 10.0f));
    float lastX = (float)SCR_WIDTH / 2.0;
    float lastY = (float)SCR_HEIGHT / 2.0;
    bool firstMouse = true;



    //----------------------- ֡ʱ�����
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    // FPS ���
    int frameCnt = 0;
    float timeCost = 0;
}
