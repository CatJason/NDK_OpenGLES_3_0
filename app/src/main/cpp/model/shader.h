#ifndef SHADER_H
#define SHADER_H

#include <glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLUtils.h>
#include <LogUtil.h>

// 着色器类，用于管理和操作OpenGL着色器程序
class Shader
{
public:
    unsigned int ID; // 着色器程序ID

    // 构造函数，通过顶点和片段着色器源码生成着色器程序
    Shader(const char* vertexStr, const char* fragmentStr)
    {
        DEBUG_LOGCATE();
        ID = GLUtils::CreateProgram(vertexStr, fragmentStr); // 创建着色器程序
    }

    // 析构函数
    ~Shader()
    {
        DEBUG_LOGCATE();
    }

    // 销毁着色器程序
    void Destroy()
    {
        GLUtils::DeleteProgram(ID); // 删除着色器程序
    }

    // 激活当前着色器程序
    void use()
    {
        glUseProgram(ID);
    }

    // 设置布尔值
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // 设置整型值
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // 设置浮点值
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // 设置二维向量
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    // 设置二维向量
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // 设置三维向量
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    // 设置三维向量
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // 设置四维向量
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    // 设置四维向量
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // 设置2x2矩阵
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // 设置3x3矩阵
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // 设置4x4矩阵
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

#endif