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

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* 1. 帧缓冲 FrameBuffer */
// 我们已经使用了很多屏幕缓冲了(颜色缓冲, 深度缓冲, 模板缓冲)
// 这些缓冲结合起来叫做帧缓冲(Framebuffer), 它被储存在内存中
// OpenGL 允许我们定义我们自己的帧缓冲
// 也就是说我们能够定义我们自己的颜色缓冲甚至是深度缓冲和模板缓冲

// 这里的例子把颜色缓冲放在纹理附件里, 把深度和模板缓冲放在渲染缓冲对象里


string screenShaders[2] = {
    "5.1.framebuffers_screen.vs",
    "5.1.framebuffers_screen.fs"
};
int main(int argc, char** argv) {
    cout << "choose your vs/fs with args\nex: Demo.exe inversion.vs inversion.fs" << endl;
    if (argc == 3) {
        screenShaders[0] = string(argv[1]);
        screenShaders[1] = string(argv[2]);
    }
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

    // build and compile shaders
    // -------------------------
    Shader shader("5.1.framebuffers.vs", "5.1.framebuffers.fs");
    Shader screenShader(screenShaders[0].c_str(), screenShaders[1].c_str());

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
        // positions          // texture Coords 
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
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
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture("container.jpg");
    unsigned int floorTexture = loadTexture("metal.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // framebuffer configuration
    // -------------------------
    // 创建一个帧缓冲 FrameBuffer Object(FBO)
    // 一个完整的帧缓冲需要满足以下的条件
    // (1) 附加至少一个缓冲(颜色、深度或模板缓冲)
    // (2) 至少有一个颜色附件(Attachment)
    // (3) 所有的附件都必须是完整的(保留了内存)
    // (4) 每个缓冲都应该有相同的样本数(后面会讲)
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    // 绑定到 GL_FRAMEBUFFER 目标之后, 所有的读取和写入帧缓冲的操作将会影响当前绑定的帧缓冲
    // GL_READ_FRAMEBUFFER：
        // 将一个帧缓冲分别绑定到读取目标
        // 将会使用在所有像是glReadPixels的读取操作
    // GL_DRAW_FRAMEBUFFER:
        // 将一个帧缓冲分别绑定到写入目标
        // 将会被用作渲染、清除等写入操作的目标
    // 大部分情况你都不需要区分它们, 通常都会使用 GL_FRAMEBUFFER, 绑定到两个上

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); // 绑定

    // 在完整性检查执行之前, 我们需要给帧缓冲附加一个附件
    // 附件是一个内存位置, 它能够作为帧缓冲的一个缓冲, 可以将它想象为一个图像
    // 当创建一个附件的时候我们有两个选项: 纹理或渲染缓冲对象(Renderbuffer Object)

    // create a color attachment texture 纹理附件
    // 当把一个纹理附加到帧缓冲的时候, 所有的渲染指令将会写入到这个纹理中
    // 想象它是一个普通的颜色/深度或模板缓冲一样
    // 使用纹理的优点是: 所有渲染操作的结果将会被储存在一个纹理图像中
    // 我们之后可以在着色器中很方便地使用它

    // 为帧缓冲创建一个纹理和创建一个普通的纹理差不多
    // 主要的区别就是, 我们将维度设置为了屏幕大小(尽管这不是必须的)
    // 并且我们给纹理的 data 参数传递了 NULL
    // 对于这个纹理，我们仅仅分配了内存而没有填充它
    // 填充这个纹理将会在我们渲染到帧缓冲之后来进行
    // 同样注意我们并不关心环绕方式或多级渐远纹理, 我们在大多数情况下都不会需要它们(大小一致)
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    // 创建纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 将纹理附件绑定到帧缓冲上
    // 参数如下
    // target: 帧缓冲的目标(绘制、读取或者两者皆有)
    // attachment: 我们想要附加的附件类型
        // 当前我们正在附加一个颜色附件
        // 注意最后的0意味着我们可以附加多个颜色附件
    // textarget: 你希望附加的纹理类型 
    // texture: 要附加的纹理本身
    // level: 多级渐远纹理的级别, 我们将它保留为 0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // 除了颜色附件之外, 我们还可以附加一个深度和模板缓冲纹理到帧缓冲对象中
    // (1) 深度缓冲
        // 附件类型: GL_DEPTH_ATTACHMENT
        // 纹理的格式(Format)和内部格式(Internalformat)类型: GL_DEPTH_COMPONENT
    // (2) 模板缓冲
        // 附件类型 GL_STENCIL_ATTACHMENT
        // 纹理格式: GL_STENCIL_INDEX
    // (3) 将深度缓冲和模板缓冲附加为一个单独的纹理
        // 纹理的每32位数值将包含24位的深度信息和8位的模板信息
        // 附件类型: GL_DEPTH_STENCIL_ATTACHMENT
        // 具体配置如下
    /*
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
            GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
        );
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    */

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    // 渲染缓冲对象附件
    // 渲染缓冲对象(Renderbuffer Object)是在纹理之后引入到 OpenGL 中
    // 作为一个可用的帧缓冲附件类型的, 所以在过去纹理是唯一可用的附件
    // 和纹理图像一样, 渲染缓冲对象是一个真正的缓冲, 即一系列的字节、整数、像素等
    // 渲染缓冲对象附加的好处是, 它会将数据储存为 OpenGL 原生的渲染格式, 它是为离屏渲染到帧缓冲优化过的

    // 渲染缓冲对象直接将所有的渲染数据储存到它的缓冲中, 不会做任何针对纹理格式的转换
    // 让它变为一个更快的可写储存介质
    // 然而, 渲染缓冲对象通常都是只写的, 所以你不能读取它们(比如使用纹理访问)
    // 当然你仍然还是能够使用 glReadPixels 来读取它, 这会从当前绑定的帧缓冲
    // 而不是附件本身, 中返回特定区域的像素

    // 因为它的数据已经是原生的格式了, 当写入或者复制它的数据到其它缓冲中时是非常快的
    // 所以, 交换缓冲这样的操作在使用渲染缓冲对象时会非常快
    // 我们在每个渲染迭代最后使用的 glfwSwapBuffers
    // 也可以通过渲染缓冲对象实现, 只需要写入一个渲染缓冲图像
    // 在最后交换到另外一个渲染缓冲就可以了
    // 渲染缓冲对象对这种操作非常完美。

    // 由于渲染缓冲对象通常都是只写的, 它们会经常用于深度和模板附件
    // 因为大部分时间我们都不需要从深度和模板缓冲中读取值, 只关心深度和模板测试
    // 我们需要深度和模板值用于测试, 但不需要对它们进行采样, 所以渲染缓冲对象非常适合它们
    // 当我们不需要从这些缓冲中采样的时候, 通常都会选择渲染缓冲对象, 因为它会更优化一点
    
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // use a single renderbuffer object for both a depth AND stencil buffer.
    // 创建一个深度和模板渲染缓冲对象
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    // now actually attach it
    // 附加到帧缓冲上
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // 渲染缓冲对象能为你的帧缓冲对象提供一些优化
    // 但知道什么时候使用渲染缓冲对象, 什么时候使用纹理是很重要的
    // 通常的规则是: 
    // 如果你不需要从一个缓冲中采样数据, 那么对这个缓冲使用渲染缓冲对象会是明智的选择
    // 如果你需要从缓冲中采样颜色或深度值等数据, 那么你应该选择纹理附件
    // 性能方面它不会产生非常大的影响的

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    // 判断我们的帧缓冲是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    }

    // 之后所有的渲染操作将会渲染到当前绑定帧缓冲的附件中
    // 由于我们的帧缓冲不是默认帧缓冲, 渲染指令将不会对窗口的视觉输出有任何影响
    // 出于这个原因, 渲染到一个不同的帧缓冲被叫做离屏渲染(Off-screen Rendering)
    // 要保证所有的渲染操作在主窗口中有视觉效果, 我们需要再次激活默认帧缓冲, 将它绑定到 0
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解绑

    // draw as wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        // bind to framebuffer and draw scene as we normally would to color texture 
        // 渲染到纹理
        // 绑定帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // make sure we clear the framebuffer's content
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
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.0f)); // z-fighting
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        // 此时已经将整个图像渲染到帧缓冲的纹理上

        // (1) 利用纹理附件绘制
        // 使用上述纹理绘制一个屏幕大小的矩形
        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        // use the color attachment texture as the texture of the quad plane
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        //// (2) 利用渲染缓冲对象交换缓冲
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        //// 拷贝操作
        //int width_now, height_now;
        //glfwGetWindowSize(window, &width_now, &height_now);
        //glBlitFramebuffer(
        //    0, 0, SCR_WIDTH - 1, SCR_HEIGHT - 1,
        //    0, 0, width_now - 1, height_now - 1,
        //    GL_COLOR_BUFFER_BIT, GL_NEAREST
        //);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(1, &framebuffer);
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