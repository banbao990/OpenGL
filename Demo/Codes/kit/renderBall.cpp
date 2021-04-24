void renderBall(const int XSEG = 40, const int YSEG = 30, bool generateNormal = true);


// renderBall() 绘制一个球体, 半径为 1.0
// 可以不生成法线数据, 和顶点数据一致
#define MY_PI 3.1415926f
unsigned int ballVAO = 0;
unsigned int ballVBO = 0;
unsigned int ballEBO = 0;
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