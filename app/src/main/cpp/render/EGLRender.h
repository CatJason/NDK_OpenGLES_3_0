//
// Created by ByteFlow on 2019/7/17.
//

#ifndef NDK_OPENGLES_3_0_BGRENDER_H
#define NDK_OPENGLES_3_0_BGRENDER_H

#include "stdint.h"
#include <GLES3/gl3.h>
#include <ImageDef.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define EGL_FEATURE_NUM 7 // 着色器特效数量

// EGLRender 类，用于处理 OpenGL ES 和 EGL 环境的初始化和渲染
class EGLRender
{
public:
    // 构造函数
    EGLRender();

    // 析构函数
    ~EGLRender();

    /**
     * 初始化 EGL 渲染器
     */
    void Init();

    /**
     * 创建 OpenGL ES 环境
     * @return 0 表示成功，其他值表示失败
     */
    int CreateGlesEnv();

    /**
     * 设置图像数据
     * @param pData 图像数据指针
     * @param width 图像宽度
     * @param height 图像高度
     */
    void SetImageData(uint8_t *pData, int width, int height);

    /**
     * 设置整型参数
     * @param paramType 参数类型
     * @param param 参数值
     */
    void SetIntParams(int paramType, int param);

    /**
     * 执行渲染操作
     */
    void Draw();

    /**
     * 销毁 OpenGL ES 环境
     */
    void DestroyGlesEnv();

    /**
     * 释放资源
     */
    void UnInit();

    /**
     * 获取单例实例
     * @return EGLRender 的单例实例
     */
    static EGLRender* GetInstance()
    {
        if (m_Instance == nullptr)
        {
            m_Instance = new EGLRender();
        }

        return m_Instance;
    }

    /**
     * 销毁单例实例
     */
    static void DestroyInstance()
    {
        if (m_Instance)
        {
            delete m_Instance;
            m_Instance = nullptr;
        }
    }

private:
    static EGLRender *m_Instance;          // 单例实例
    GLuint m_ImageTextureId;               // 图像纹理 ID
    GLuint m_FboTextureId;                 // FBO 纹理 ID
    GLuint m_FboId;                        // FBO ID
    GLuint m_VaoIds[1] = {GL_NONE};        // VAO ID
    GLuint m_VboIds[3] = {GL_NONE};        // VBO IDs
    GLint m_SamplerLoc;                    // 纹理采样器位置
    GLint m_TexSizeLoc;                    // 纹理尺寸位置
    NativeImage m_RenderImage;             // 渲染的图像数据
    GLuint m_ProgramObj;                   // 着色器程序对象 ID
    GLuint m_VertexShader;                 // 顶点着色器对象 ID
    GLuint m_FragmentShader;               // 片段着色器对象 ID

    EGLConfig  m_eglConf;                  // EGL 配置
    EGLSurface m_eglSurface;               // EGL 表面
    EGLContext m_eglCtx;                   // EGL 上下文
    EGLDisplay m_eglDisplay;               // EGL 显示
    bool       m_IsGLContextReady;         // OpenGL 上下文是否已准备好
    const char*m_fShaderStrs[EGL_FEATURE_NUM]; // 着色器字符串数组
    int        m_ShaderIndex;              // 当前使用的着色器索引
};

#endif //NDK_OPENGLES_3_0_BGRENDER_H
