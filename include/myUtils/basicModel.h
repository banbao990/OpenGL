#pragma once

#ifndef MY_PI
#define MY_PI 3.1415926f
#endif // !MY_PI

namespace BASIC_MODEL {
// VAO,VBO,EBO
    GLuint ballVAO = 0;
    GLuint ballVBO = 0;
    GLuint ballEBO = 0;
    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;
    GLuint planeVAO = 0;
    GLuint planeVBO = 0;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;

    // 渲染基本几何体函数
    void renderQuad();
    void renderCube();
    void renderPlane();
    void renderBall(const int XSEG = 100, const int YSEG = 100, bool generateNormal = true);

    // 渲染一个球体 R = 1
    void renderBall(const int XSEG, const int YSEG, bool generateNormal) {
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
                float x = cos(1.0f*i / XSEG * 2 * MY_PI), y = sin(1.0f*i / XSEG * 2 * MY_PI);
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
            // std::cout << "Total nods in ball: " << totalNodes << " = " << ++num << std::endl;
            if (generateNormal) {
                // 法线数据(和顶点数据一致)
                memcpy(nodes + totalNodes, nodes, sizeof(nodes[0])*totalNodes);
            }
            // 生成引用数据
            GLuint* indices = new GLuint[totalIndices * 3];
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
            // std::cout << "Total nodes in ball:(3*)" << totalIndices * 3 << " = " << num << std::endl;
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

    // 渲染一个立方体 [-1, 1]^3
    void renderCube() {
        if (cubeVAO == 0) {
            float vertices[] = {
                // back face         
                //position            // normal           // texture coordinate 
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
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
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  // bottom-left        
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
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

    // 渲染一个平面, 正方体下表面 [-1, 1]^2
    void renderPlane() {
        if (planeVAO == 0) {
            float vertices[] = {
                 //position            // normal           // texture coordinate
                -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  0.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                 1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  // bottom-left        
            };
            glGenVertexArrays(1, &planeVAO);
            glGenBuffers(1, &planeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(planeVAO);
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
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    // 渲染整个屏幕, 一般用于图像后期处理, 屏幕空间 [-1, 1]^2
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
}