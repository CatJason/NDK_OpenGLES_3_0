#ifndef MODEL_H
#define MODEL_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../inc/assimp/Importer.hpp"
#include "../inc/assimp/scene.h"
#include "../inc/assimp/postprocess.h"
#include <shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <opencv2/opencv.hpp>
#include "mesh.h"
#include "LogUtil.h"

using namespace std;

// 模型类，用于加载和渲染3D模型
class Model
{
public:
    /* 模型数据 */
    vector<Texture> textures_loaded;	// 已加载的纹理，避免重复加载
    vector<Mesh> meshes;                // 模型中的网格列表
    string directory;                   // 模型所在的目录
    glm::vec3 maxXyz, minXyz;           // 模型的最大和最小坐标，用于计算包围盒
    bool gammaCorrection;               // 是否应用Gamma校正
    bool hasTexture;                    // 模型是否包含纹理

    /* 构造函数 */
    // 构造函数，接受模型文件路径
    Model(string const &path, bool gamma = false) :
            gammaCorrection(gamma),
            hasTexture(false)
    {
        loadModel(path); // 加载模型
    }

    /* 渲染模型 */
    // 渲染模型中的所有网格
    void Draw(Shader shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    /* 获取模型视距 */
    // 获取模型的最大可视距离
    float GetMaxViewDistance()
    {
        glm::vec3 vec3 = (abs(minXyz) + abs(maxXyz)) / 2.0f;
        float maxDis = fmax(vec3.x, fmax(vec3.y, vec3.z));
        LOGCATE("Model::GetMaxViewDistance maxDis=%f", maxDis);
        return maxDis;
    }

    /* 获取模型的调整位置向量 */
    glm::vec3 GetAdjustModelPosVec()
    {
        glm::vec3 vec3 = (minXyz + maxXyz) / 2.0f;
        LOGCATE("Model::GetAdjustModelPosVec vec3(%f, %f, %f)", vec3.x, vec3.y, vec3.z);
        return (minXyz + maxXyz) / 2.0f;
    }

    /* 判断是否包含纹理 */
    bool ContainsTextures()
    {
        return hasTexture;
    }

    /* 销毁模型资源 */
    void Destroy()
    {
        for (Mesh &mesh : meshes) {
            mesh.Destroy(); // 销毁每个网格的资源
        }
    }

private:
    /* 加载模型文件 */
    void loadModel(string const &path)
    {
        // 使用ASSIMP加载模型文件
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        DEBUG_LOGCATE();
        // 检查加载是否成功
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOGCATE("Model::loadModel path=%s, assimpError=%s", path.c_str(), importer.GetErrorString());
            return;
        }
        DEBUG_LOGCATE();
        // 获取文件的目录
        directory = path.substr(0, path.find_last_of('/'));

        // 递归处理根节点
        processNode(scene->mRootNode, scene);
    }

    /* 递归处理节点 */
    void processNode(aiNode *node, const aiScene *scene)
    {
        // 处理当前节点的所有网格
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            if(mesh != nullptr)
                meshes.push_back(processMesh(mesh, scene));
        }
        DEBUG_LOGCATE();
        // 递归处理子节点
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    /* 更新模型的最大最小坐标 */
    void updateMaxMinXyz(glm::vec3 pos)
    {
        maxXyz.x = pos.x > maxXyz.x ? pos.x : maxXyz.x;
        maxXyz.y = pos.y > maxXyz.y ? pos.y : maxXyz.y;
        maxXyz.z = pos.z > maxXyz.z ? pos.z : maxXyz.z;

        minXyz.x = pos.x < minXyz.x ? pos.x : minXyz.x;
        minXyz.y = pos.y < minXyz.y ? pos.y : minXyz.y;
        minXyz.z = pos.z < minXyz.z ? pos.z : minXyz.z;
    }

    /* 处理网格 */
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // 遍历网格的顶点
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            // 读取顶点位置
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            updateMaxMinXyz(vector);

            // 读取法线
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            // 读取纹理坐标
            if(mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            // 读取切线和副切线
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;

            vertices.push_back(vertex);
        }

        // 遍历网格的面
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // 处理材质
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // 返回网格对象
        return Mesh(vertices, indices, textures);
    }

    /* 加载材质纹理 */
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        DEBUG_LOGCATE();
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if(!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    /* 从文件加载纹理 */
    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false)
    {
        string filename = string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = nullptr;

        // 使用OpenCV加载纹理
        LOGCATE("TextureFromFile Loading texture %s", filename.c_str());
        cv::Mat textureImage = cv::imread(filename);
        if (!textureImage.empty())
        {
            hasTexture = true;
            cv::cvtColor(textureImage, textureImage, CV_BGR2RGB);

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureImage.cols,
                         textureImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         textureImage.data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            GO_CHECK_GL_ERROR();
        } else {
            LOGCATE("TextureFromFile Texture failed to load at path: %s", path);
        }

        return textureID;
    }
};

#endif
