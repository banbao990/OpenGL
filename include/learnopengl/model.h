// learnopengl 模型类(Assimp)
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <learnopengl/mesh.h>
#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

// 模型类
class Model {
public:
    // model data 模型数据
    // 保存已经加载的纹理, 保证只会被加载一次(优化)
    // stores all the textures loaded so far, 
    // optimization to make sure textures aren't loaded more than once.
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes; // 所有的网格
    string directory;
    bool gammaCorrection; // gamma 颜色矫正

    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file 
    // and stores the resulting meshes in the meshes vector.
    // 利用 Assimp 类加载模型(参数为路径)
    void loadModel(string const &path) {
        // read file via ASSIMP
        // 一些加载参数 http://assimp.sourceforge.net/lib_html/postprocess_8h.html
        // aiProcess_Triangulate: 如果模型不是(全部)由三角形组成, 则将模型所有的图元形状变换为三角形
        // aiProcess_FlipUVs: 将在处理的时候翻转 y 轴的纹理坐标(向上增长)
        // aiProcess_GenNormals: 如果模型不包含法向量的话，就为每个顶点创建法线
        // aiProcess_SplitLargeMeshes: 将比较大的网格分割成更小的子网格
            // 如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用
        // aiProcess_OptimizeMeshes: 它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化
        // aiProcess_CalcTangentSpace: Calculates the tangents and bitangents for the imported meshes
        Assimp::Importer importer;
        // Assimp有个很有用的配置, 在我们加载模型的时候调用aiProcess_CalcTangentSpace
        // 当aiProcess_CalcTangentSpace应用到Assimp的ReadFile函数时
        // Assimp会为每个加载的顶点计算出柔和的切线和副切线向量
        // 它所使用的方法和我们本教程使用的类似
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors(if is Not Zero) 检查模型读取是否出错
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        // 递归处理读取进来的模型(根结点)
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion.
    // Processes each individual mesh located at the node 
    // and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene) {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // 结点中只是包含对于实际网格的索引信息, 完整信息保存在 scene 的结构体中
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    // 处理网格, 转换为自定义的 Mesh 类 (aiMesh -> Mesh)
    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        // data to fill(Mesh 类的构造参数)
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            // we declare a placeholder vector
            // since assimp uses its own vector class that doesn't directly convert to glm's vec3 class
            // so we transfer the data to this placeholder glm::vec3 first.
            glm::vec3 vector;
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            // does the mesh contain texture coordinates?
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates.
                // We thus make the assumption that we won't use models where a vertex 
                // can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            } else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // now walk through each of the mesh's faces 
        // (a face is a mesh its triangle) 
        // and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        // wavefront的模型格式(.obj)导出的法线贴图有点不一样
        // Assimp的aiTextureType_NORMAL并不会加载它的法线贴图
        // 而aiTextureType_HEIGHT却能, 所以我们经常这样加载它们
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // 寻找对应 type 的纹理
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before.
            // if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip) {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                // store it as texture loaded for entire model,
                // to ensure we won't unnecesery load duplicate textures.
                textures_loaded.push_back(texture);  
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma) {
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
#endif