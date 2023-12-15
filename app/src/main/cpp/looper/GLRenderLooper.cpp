#include <GLUtils.h>
#include "GLRenderLooper.h"

// 初始化静态变量
mutex GLRenderLooper::m_Mutex;
GLRenderLooper* GLRenderLooper::m_Instance = nullptr;

// 析构函数
GLRenderLooper::~GLRenderLooper() {}

// 构造函数
GLRenderLooper::GLRenderLooper() {}

/**
 * 处理消息
 * @param msg 消息指针
 */
void GLRenderLooper::handleMessage(LooperMessage *msg) {
    Looper::handleMessage(msg);
    switch (msg->what) {
        case MSG_SurfaceCreated: {
            LOGCATE("GLRenderLooper::handleMessage MSG_SurfaceCreated");
            m_GLEnv = (GLEnv *)msg->obj; // 保存GL环境
            OnSurfaceCreated(); // 处理Surface创建
        }
            break;
        case MSG_SurfaceChanged:
            LOGCATE("GLRenderLooper::handleMessage MSG_SurfaceChanged");
            OnSurfaceChanged(msg->arg1, msg->arg2); // 处理Surface尺寸改变
            break;
        case MSG_DrawFrame:
            LOGCATE("GLRenderLooper::handleMessage MSG_DrawFrame");
            OnDrawFrame(); // 绘制帧
            break;
        case MSG_SurfaceDestroyed:
            LOGCATE("GLRenderLooper::handleMessage MSG_SurfaceDestroyed");
            OnSurfaceDestroyed(); // 处理Surface销毁
            break;
        default:
            break;
    }
}

/**
 * 获取单例实例
 * @return GLRenderLooper 单例对象
 */
GLRenderLooper *GLRenderLooper::GetInstance() {
    LOGCATE("GLRenderLooper::GetInstance");
    if (m_Instance == nullptr) {
        unique_lock<mutex> lock(m_Mutex); // 线程安全
        if (m_Instance == nullptr) {
            m_Instance = new GLRenderLooper(); // 创建实例
        }
    }
    return m_Instance;
}

/**
 * 释放单例实例
 */
void GLRenderLooper::ReleaseInstance() {
    LOGCATE("GLRenderLooper::ReleaseInstance");
    if (m_Instance != nullptr) {
        unique_lock<mutex> lock(m_Mutex); // 线程安全
        if (m_Instance != nullptr) {
            delete m_Instance; // 删除实例
            m_Instance = nullptr;
        }
    }
}

/**
 * 处理Surface创建
 */
void GLRenderLooper::OnSurfaceCreated() {
    LOGCATE("GLRenderLooper::OnSurfaceCreated");
    // 初始化 EGL 环境
    m_EglCore = new EglCore(m_GLEnv->sharedCtx, FLAG_RECORDABLE);
    SizeF imgSizeF = m_GLEnv->imgSize;
    m_OffscreenSurface = new OffscreenSurface(m_EglCore, imgSizeF.width, imgSizeF.height);
    m_OffscreenSurface->makeCurrent();

    // 初始化顶点数组对象 (VAO) 和顶点缓冲对象 (VBO)
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_GLEnv->vboIds[0]);
    glEnableVertexAttribArray(0); // 启用顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0); // 设置顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_GLEnv->vboIds[1]);
    glEnableVertexAttribArray(1); // 启用纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLEnv->vboIds[2]); // 绑定元素缓冲对象
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    // 创建帧缓冲对象 (FBO)
    if (!CreateFrameBufferObj()) {
        LOGCATE("GLRenderLooper::OnSurfaceCreated CreateFrameBufferObj fail");
    }
}

/**
 * 处理Surface尺寸改变
 * @param w 宽度
 * @param h 高度
 */
void GLRenderLooper::OnSurfaceChanged(int w, int h) {
    LOGCATE("GLRenderLooper::OnSurfaceChanged [w,h]=[%d, %d]", w, h);
}

/**
 * 绘制帧
 */
void GLRenderLooper::OnDrawFrame() {
    LOGCATE("GLRenderLooper::OnDrawFrame");
    SizeF imgSizeF = m_GLEnv->imgSize;

    // 绑定 FBO 进行渲染
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glViewport(0, 0, imgSizeF.width, imgSizeF.height);
    glUseProgram(m_GLEnv->program); // 使用渲染程序
    glBindVertexArray(m_VaoId);

    glActiveTexture(GL_TEXTURE0); // 激活纹理单元
    glBindTexture(GL_TEXTURE_2D, m_GLEnv->inputTexId); // 绑定输入纹理
    GLUtils::setInt(m_GLEnv->program, "s_TextureMap", 0); // 设置纹理采样器
    float offset = (sin(m_FrameIndex * MATH_PI / 80) + 1.0f) / 2.0f; // 计算偏移量
    GLUtils::setFloat(m_GLEnv->program, "u_Offset", offset); // 设置偏移量
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0); // 绘制元素
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_OffscreenSurface->swapBuffers(); // 交换缓冲区
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 渲染完成回调
    m_GLEnv->renderDone(m_GLEnv->callbackCtx, m_FboTextureId);
    m_FrameIndex++;
}

/**
 * 处理Surface销毁
 */
void GLRenderLooper::OnSurfaceDestroyed() {
    LOGCATE("GLRenderLooper::OnSurfaceDestroyed");
    m_GLEnv->renderDone(m_GLEnv->callbackCtx, m_FboTextureId);

    // 删除 VAO
    if (m_VaoId) {
        glDeleteVertexArrays(1, &m_VaoId);
    }

    // 删除 FBO
    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
    }

    // 删除 FBO 纹理
    if (m_FboTextureId) {
        glDeleteTextures(1, &m_FboTextureId);
    }

    // 释放离屏 Surface
    if (m_OffscreenSurface) {
        m_OffscreenSurface->release();
        delete m_OffscreenSurface;
        m_OffscreenSurface = nullptr;
    }

    // 释放 EGL 环境
    if (m_EglCore) {
        m_EglCore->release();
        delete m_EglCore;
        m_EglCore = nullptr;
    }
}

/**
 * 创建帧缓冲对象 (FBO)
 * @return 是否成功创建
 */
bool GLRenderLooper::CreateFrameBufferObj() {
    // 创建并初始化 FBO 纹理
    glGenTextures(1, &m_FboTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 创建并初始化 FBO
    glGenFramebuffers(1, &m_FboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GLEnv->imgSize.width, m_GLEnv->imgSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGCATE("RGB2YUYVSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}
