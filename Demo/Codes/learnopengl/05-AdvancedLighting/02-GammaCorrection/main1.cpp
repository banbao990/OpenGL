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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool blinn = false;
bool blinnKeyPressed = false;
bool lightPause = false;
bool lightPauseKeyPressed = false;
bool gammaCorrection = false;
bool gammaCorrectionKeyPressed = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* Gamma 校正 */
// Gamma也叫灰度系数, 每种显示设备都有自己的 Gamma 值, 都不相同
// 有一个公式: 设备输出亮度 = 电压的 Gamma 次幂
// 任何设备 Gamma 基本上都不会等于 1, 等于1是一种理想的线性状态
// 这种理想状态是: 如果电压和亮度都是在0到1的区间, 那么多少电压就等于多少亮度
// 对于CRT, Gamma通常为2.2
// Gamma2.2 实际显示出来的总会比预期暗, 相反 Gamma0.45 就会比理想预期亮
// 如果你讲 Gamma0.45 叠加到 Gamma2.2 的显示设备上, 便会对偏暗的显示效果做到校正
// 这个简单的思路就是本节的核心

// 人类所感知的亮度恰好和 CRT 所显示出来相似的指数关系非常匹配

/* 1. gamma 2.2 */
// 2.2 通常是是大多数显示设备的大概平均 gamma 值
// 基于 gamma2.2 的颜色空间叫做sRGB颜色空间
// 每个监视器的 gamma 曲线都有所不同, 但是gamma2.2在大多数监视器上表现都不错
// 出于这个原因, 游戏经常都会为玩家提供改变游戏 gamma 设置的选项, 以适应每个监视器
// (译注：现在 Gamma2.2 相当于一个标准, 后文中你会看到, 但现在你可能会问
// 前面不是说 Gamma2.2 看起来不是正好适合人眼么, 为何还需要校正
// 这是因为你在程序中设置的颜色, 比如光照都是基于线性 Gamma(Gamma1)
// 所以你理想中的亮度和实际表达出的不一样, 如果要表达出你理想中的亮度就要对这个光照进行校正

/* 2. gamma 校正的两种方法 */
// (1) glEnable(GL_FRAMEBUFFER_SRGB)
// Fragment Shader时进行 gamma correction (因此在调用 glDraw 之前需要开启)
// 开启 GL_FRAMEBUFFER_SRGB 以后, 每次像素着色器运行后续帧缓冲
// OpenGL 将自动执行 gamma 校正, 包括默认帧缓冲

// 注意开启的时机(最后一步)
// gamma 校正将把线性颜色空间转变为非线性空间, 所以在最后一步进行 gamma 校正是极其重要的
// 如果你在最后输出之前就进行 gamma 校正, 所有的后续操作都是在操作不正确的颜色值
// 例如, 如果你使用多个帧缓冲, 你可能打算让两个帧缓冲之间传递的中间结果仍然保持线性空间颜色
// 只是给发送给监视器的最后的那个帧缓冲应用 gamma 校正

// (2) Fragment Shader 中手动计算

/* 一些其他问题 */
// 多次矫正
// 因为监视器总是在sRGB空间中显示应用了gamma的颜色
// 无论什么时候当你在计算机上绘制, 编辑或者画出一个图片的时候
// 你所选的颜色都是根据你在监视器上看到的那种
// 这实际意味着所有你创建或编辑的图片并不是在线性空间, 而是在sRGB空间中
// 假如在你的屏幕上对暗红色翻一倍, 便是根据你所感知到的亮度进行的, 并不等于将红色元素加倍

// 结果就是纹理编辑者, 所创建的所有纹理都是在sRGB空间中的纹理
// 所以如果我们在渲染应用中使用这些纹理, 我们必须考虑到这点
// 在我们应用 gamma 校正之前, 这不是个问题, 因为纹理在sRGB空间创建和展示
// 同样我们还是在sRGB空间中使用, 从而不必gamma校正纹理显示也没问题
// 然而, 现在我们是把所有东西都放在线性空间中展示的, 纹理颜色就会变坏

// 如果我们再次进行伽马校正
// 纹理图像实在太亮了, 发生这种情况是因为, 它们实际上进行了两次 gamma 校正
// 想一想, 当我们基于监视器上看到的情况创建一个图像, 我们就已经对颜色值进行了gamma校正
// 所以再次显示在监视器上就没错
// 由于我们在渲染中又进行了一次 gamma 校正, 图片就实在太亮了

// 为了修复这个问题, 我们得确保纹理制作者是在线性空间中进行创作的
// 但是, 由于大多数纹理制作者并不知道什么是gamma校正
// 并且在sRGB空间中进行创作更简单, 这也许不是一个好办法。

// 另一个解决方案是重校, 或把这些sRGB纹理在进行任何颜色值的计算前变回线性空间
// 我们可以这样做：
/*
float gamma = 2.2;
vec3 diffuseColor = pow(texture(diffuse, texCoords).rgb, vec3(gamma));
*/
// 为每个sRGB空间的纹理做这件事非常烦人
// 幸好, OpenGL给我们提供了另一个方案来解决我们的麻烦
// 这就是 GL_SRGB 和 GL_SRGB_ALPHA 内部纹理格式

// 如果我们在OpenGL中创建了一个纹理, 把它指定为以上两种sRGB纹理格式其中之一
// OpenGL将自动把颜色校正到线性空间中, 这样我们所使用的所有颜色值都是在线性空间中的了
// 我们可以这样把一个纹理指定为一个sRGB纹理
/*
glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
*/
// 如果你还打算在你的纹理中引入alpha元素, 必须将纹理的内部格式指定为 GL_SRGB_ALPHA

// 因为不是所有纹理都是在sRGB空间中的所以当你把纹理指定为sRGB纹理时要格外小心
// 比如diffuse纹理, 这种为物体上色的纹理几乎都是在sRGB空间中的
// 而为了获取光照参数的纹理, 像specular贴图和法线贴图几乎都在线性空间中
// 所以如果你把它们也配置为sRGB纹理的话, 光照就坏掉了
// 指定sRGB纹理时要当心

// 将diffuse纹理定义为sRGB纹理之后, 你将获得你所期望的视觉输出, 但这次每个物体都会只进行一次gamma校正

int main(int argc, char** argv) {
    string whichShader("1");
    if (argc == 2) {
        whichShader = string(argv[1]);
    }
    cout <<
        "Demo.exe (whichShader)\nEx Demo.exe 1\n\n"
        "Press B to change Lighting Model(Phong/Blinn-Phong)\n"
        "Press Space to stop/start the light\n"
        "Press G to on/off the gamma correction(sRGB)\n"
        << endl;

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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader shader(
        (whichShader + ".vs").c_str(),
        (whichShader + ".fs").c_str()
    );
    Shader lightShader("light.vs", "light.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // light cube
    float vertices[] = {
       // position
       -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,

       -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f, -0.5f,  0.5f,

       -0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

       -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
       -0.5f, -0.5f,  0.5f,
       -0.5f, -0.5f, -0.5f,

       -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f, -0.5f
    };

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // light cube
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int floorTexture = loadTexture("wood.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    // lighting info
    // -------------
    glm::vec3 lightPos(1.0f, 0.5f, 0.0f);
    float lightTime = 0.0f;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        if (gammaCorrection) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        } else {
            glDisable(GL_FRAMEBUFFER_SRGB);
        }

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // update lightinh position
        if (!lightPause) {
            lightTime += 0.01f;
            lightPos.x = cos(lightTime);
            lightPos.z = sin(lightTime);
        }

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw objects
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setInt("blinn", blinn);
        // floor
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // draw light
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        lightShader.setMat4("model", model);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);

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
        std::cout << "Light" << (lightPause ? "Stop" : "Start") << std::endl;
        lightPauseKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        lightPauseKeyPressed = false;
    }

    // gamma correction or not
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gammaCorrectionKeyPressed) {
        gammaCorrection = !gammaCorrection;
        std::cout << "Gamma Correction(sRGB) " << (gammaCorrection ? "On" : "Off") << std::endl;
        gammaCorrectionKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
        gammaCorrectionKeyPressed = false;
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