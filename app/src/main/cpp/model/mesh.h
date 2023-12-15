#ifndef MESH_H
#define MESH_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "../glm/vec3.hpp"

using namespace std;

// 顶点结构体，包含顶点的各种属性
struct Vertex {
    // 位置
    glm::vec3 Position;
    // 法线
    glm::vec3 Normal;
    // 纹理坐标
    glm::vec2 TexCoords;
    // 切线
    glm::vec3 Tangent;
    // 副切线
    glm::vec3 Bitangent;
};

// 纹理结构体，包含纹理的相关属性
struct Texture {
    unsigned int id;    // 纹理ID
    string type;        // 纹理类型
    string path;        // 纹理路径
};

// 网格类，用于管理顶点、索引和纹理
class Mesh {
public:
    /* 网格数据 */
    vector<Vertex> vertices;       // 顶点数据
    vector<unsigned int> indices; // 索引数据
    vector<Texture> textures;      // 纹理数据
    unsigned int VAO;              // 顶点数组对象

    /* 函数 */
    // 构造函数
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // 设置顶点缓冲和属性指针
        setupMesh();
    }

    // 渲染网格
    void Draw(Shader shader)
    {
        // 绑定纹理
        unsigned int diffuseNr  = 1; // 漫反射纹理编号
        unsigned int specularNr = 1; // 高光纹理编号
        unsigned int normalNr   = 1; // 法线纹理编号
        unsigned int heightNr   = 1; // 高度纹理编号
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // 激活对应的纹理单元
            string number;
            string name = textures[i].type; // 获取纹理类型
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);
            else if(name == "texture_normal")
                number = std::to_string(normalNr++);
            else if(name == "texture_height")
                number = std::to_string(heightNr++);

            // 设置着色器中的采样器对应的纹理单元
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id); // 绑定纹理
        }

        // 绘制网格
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 重置为默认纹理单元
        glActiveTexture(GL_TEXTURE0);
    }

    // 销毁网格资源
    void Destroy()
    {
        for (int i = 0; i < textures.size(); ++i) {
            glDeleteTextures(1, &textures[i].id); // 删除纹理
        }
        glDeleteBuffers(1, &EBO); // 删除索引缓冲对象
        glDeleteBuffers(1, &VBO); // 删除顶点缓冲对象
        glDeleteVertexArrays(1, &VAO); // 删除顶点数组对象
        VAO = EBO = VBO = GL_NONE; // 重置对象为无效值
    }

private:
    /* 渲染数据 */
    unsigned int VBO, EBO; // 顶点缓冲对象和索引缓冲对象

    /* 函数 */
    // 初始化所有缓冲对象和数组
    void setupMesh()
    {
        // 创建缓冲对象和数组
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // 将顶点数据加载到顶点缓冲对象中
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 设置顶点属性指针
        // 顶点位置
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // 顶点法线
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // 顶点纹理坐标
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // 顶点切线
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // 顶点副切线
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0); // 解绑顶点数组对象
    }
};
#endif