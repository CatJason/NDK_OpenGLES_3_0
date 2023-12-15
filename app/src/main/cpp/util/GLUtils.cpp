#include "GLUtils.h"
#include "LogUtil.h"
#include <stdlib.h>
#include <cstring>
#include <GLES2/gl2ext.h>

/**
 * 加载并编译着色器
 * @param shaderType 着色器类型（GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER）
 * @param pSource 着色器源码
 * @return 返回编译成功的着色器对象，如果失败返回 0
 */
GLuint GLUtils::LoadShader(GLenum shaderType, const char *pSource)
{
    GLuint shader = 0;
    FUN_BEGIN_TIME("GLUtils::LoadShader") // 性能分析起始点
        shader = glCreateShader(shaderType); // 创建着色器对象
        if (shader)
        {
            glShaderSource(shader, 1, &pSource, NULL); // 绑定着色器源码
            glCompileShader(shader); // 编译着色器
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled); // 检查编译状态
            if (!compiled)
            {
                GLint infoLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen); // 获取错误信息长度
                if (infoLen)
                {
                    char* buf = (char*) malloc((size_t)infoLen);
                    if (buf)
                    {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf); // 获取错误日志
                        LOGCATE("GLUtils::LoadShader Could not compile shader %d:\n%s\n", shaderType, buf);
                        free(buf);
                    }
                    glDeleteShader(shader); // 删除着色器对象
                    shader = 0;
                }
            }
        }
    FUN_END_TIME("GLUtils::LoadShader") // 性能分析结束点
    return shader;
}

/**
 * 创建程序对象
 * @param pVertexShaderSource 顶点着色器源码
 * @param pFragShaderSource 片段着色器源码
 * @param vertexShaderHandle 顶点着色器句柄
 * @param fragShaderHandle 片段着色器句柄
 * @return 返回链接成功的程序对象，如果失败返回 0
 */
GLuint GLUtils::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle)
{
    GLuint program = 0;
    FUN_BEGIN_TIME("GLUtils::CreateProgram")
        vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource); // 加载顶点着色器
        if (!vertexShaderHandle) return program;
        fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource); // 加载片段着色器
        if (!fragShaderHandle) return program;

        program = glCreateProgram(); // 创建程序对象
        if (program)
        {
            glAttachShader(program, vertexShaderHandle); // 附加顶点着色器
            CheckGLError("glAttachShader");
            glAttachShader(program, fragShaderHandle); // 附加片段着色器
            CheckGLError("glAttachShader");
            glLinkProgram(program); // 链接程序
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus); // 检查链接状态

            // 清理着色器
            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;
            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;

            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength); // 获取错误日志长度
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf); // 获取错误日志
                        LOGCATE("GLUtils::CreateProgram Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program); // 删除程序对象
                program = 0;
            }
        }
    FUN_END_TIME("GLUtils::CreateProgram")
    LOGCATE("GLUtils::CreateProgram program = %d", program);
    return program;
}

/**
 * 创建包含 Transform Feedback 的程序对象
 * @param pVertexShaderSource 顶点着色器源码
 * @param pFragShaderSource 片段着色器源码
 * @param vertexShaderHandle 顶点着色器句柄
 * @param fragShaderHandle 片段着色器句柄
 * @param varying Transform Feedback 的变量名数组
 * @param varyingCount 变量名数量
 * @return 返回链接成功的程序对象，如果失败返回 0
 */
GLuint GLUtils::CreateProgramWithFeedback(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle, GLchar const **varying, int varyingCount)
{
    GLuint program = 0;
    FUN_BEGIN_TIME("GLUtils::CreateProgramWithFeedback")
        vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource); // 加载顶点着色器
        if (!vertexShaderHandle) return program;

        fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource); // 加载片段着色器
        if (!fragShaderHandle) return program;

        program = glCreateProgram(); // 创建程序对象
        if (program)
        {
            glAttachShader(program, vertexShaderHandle); // 附加顶点着色器
            CheckGLError("glAttachShader");
            glAttachShader(program, fragShaderHandle); // 附加片段着色器
            CheckGLError("glAttachShader");

            // 设置 Transform Feedback 的变量
            glTransformFeedbackVaryings(program, varyingCount, varying, GL_INTERLEAVED_ATTRIBS);
            GO_CHECK_GL_ERROR();

            glLinkProgram(program); // 链接程序
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus); // 检查链接状态

            // 清理着色器
            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;
            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;

            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength); // 获取错误日志长度
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf); // 获取错误日志
                        LOGCATE("GLUtils::CreateProgramWithFeedback Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program); // 删除程序对象
                program = 0;
            }
        }
    FUN_END_TIME("GLUtils::CreateProgramWithFeedback")
    LOGCATE("GLUtils::CreateProgramWithFeedback program = %d", program);
    return program;
}

/**
 * 删除程序对象
 * @param program 程序对象句柄
 */
void GLUtils::DeleteProgram(GLuint &program)
{
    LOGCATE("GLUtils::DeleteProgram");
    if (program)
    {
        glUseProgram(0); // 解绑当前程序
        glDeleteProgram(program); // 删除程序对象
        program = 0;
    }
}

/**
 * 检查 OpenGL 错误
 * @param pGLOperation 当前操作描述
 */
void GLUtils::CheckGLError(const char *pGLOperation)
{
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGCATE("GLUtils::CheckGLError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
    }
}

/**
 * 创建程序对象（简化版本）
 * @param pVertexShaderSource 顶点着色器源码
 * @param pFragShaderSource 片段着色器源码
 * @return 返回创建的程序对象
 */
GLuint GLUtils::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource) {
    GLuint vertexShaderHandle, fragShaderHandle;
    return CreateProgram(pVertexShaderSource, pFragShaderSource, vertexShaderHandle, fragShaderHandle);
}