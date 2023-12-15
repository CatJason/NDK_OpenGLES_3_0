

#ifndef _BYTE_FLOW_GL_UTILS_H_
#define _BYTE_FLOW_GL_UTILS_H_

#include <GLES3/gl3.h>
#include <string>
#include <glm.hpp>

// 将宏转换为字符串
#define SHADER_TO_STRING(s) #s

// 数学常量 PI
#define MATH_PI 3.1415926535897932384626433832802

/**
 * GL 工具类，封装了一些常用的 OpenGL 操作
 */
class GLUtils {
public:
    /**
     * 加载着色器
     * @param shaderType 着色器类型（GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER）
     * @param pSource 着色器源码
     * @return 加载成功的着色器对象
     */
    static GLuint LoadShader(GLenum shaderType, const char *pSource);

    /**
     * 创建程序对象（包含顶点和片段着色器）
     * @param pVertexShaderSource 顶点着色器源码
     * @param pFragShaderSource 片段着色器源码
     * @param vertexShaderHandle 返回的顶点着色器句柄
     * @param fragShaderHandle 返回的片段着色器句柄
     * @return 创建成功的程序对象
     */
    static GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                                GLuint &vertexShaderHandle,
                                GLuint &fragShaderHandle);

    /**
     * 创建程序对象（简化版本，不返回着色器句柄）
     * @param pVertexShaderSource 顶点着色器源码
     * @param pFragShaderSource 片段着色器源码
     * @return 创建成功的程序对象
     */
    static GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource);

    /**
     * 创建包含 Transform Feedback 的程序对象
     * @param pVertexShaderSource 顶点着色器源码
     * @param pFragShaderSource 片段着色器源码
     * @param vertexShaderHandle 顶点着色器句柄
     * @param fragShaderHandle 片段着色器句柄
     * @param varying Transform Feedback 的变量名数组
     * @param varyingCount 变量名数量
     * @return 创建成功的程序对象
     */
    static GLuint CreateProgramWithFeedback(
            const char *pVertexShaderSource,
            const char *pFragShaderSource,
            GLuint &vertexShaderHandle,
            GLuint &fragShaderHandle,
            const GLchar **varying,
            int varyingCount);

    /**
     * 删除程序对象
     * @param program 程序对象句柄
     */
    static void DeleteProgram(GLuint &program);

    /**
     * 检查 OpenGL 错误
     * @param pGLOperation 当前操作描述
     */
    static void CheckGLError(const char *pGLOperation);

    // 设置 uniform 变量的方法

    static void setBool(GLuint programId, const std::string &name, bool value) {
        glUniform1i(glGetUniformLocation(programId, name.c_str()), (int) value);
    }

    static void setInt(GLuint programId, const std::string &name, int value) {
        glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
    }

    static void setFloat(GLuint programId, const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
    }

    static void setVec2(GLuint programId, const std::string &name, const glm::vec2 &value) {
        glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
    }

    static void setVec2(GLuint programId, const std::string &name, float x, float y) {
        glUniform2f(glGetUniformLocation(programId, name.c_str()), x, y);
    }

    static void setVec3(GLuint programId, const std::string &name, const glm::vec3 &value) {
        glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
    }

    static void setVec3(GLuint programId, const std::string &name, float x, float y, float z) {
        glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
    }

    static void setVec4(GLuint programId, const std::string &name, const glm::vec4 &value) {
        glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
    }

    static void setVec4(GLuint programId, const std::string &name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
    }

    static void setMat2(GLuint programId, const std::string &name, const glm::mat2 &mat) {
        glUniformMatrix2fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    static void setMat3(GLuint programId, const std::string &name, const glm::mat3 &mat) {
        glUniformMatrix3fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    static void setMat4(GLuint programId, const std::string &name, const glm::mat4 &mat) {
        glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    /**
     * 将纹理坐标转换为顶点坐标
     * @param texCoord 纹理坐标
     * @return 转换后的顶点坐标
     */
    static glm::vec3 texCoordToVertexCoord(glm::vec2 &texCoord) {
        return glm::vec3(2 * texCoord.x - 1, 1 - 2 * texCoord.y, 0);
    }
};

#endif // _BYTE_FLOW_GL_UTILS_H_