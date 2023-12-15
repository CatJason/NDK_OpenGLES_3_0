#ifndef NDK_OPENGLES_3_0_GLRENDERLOOPER_H
#define NDK_OPENGLES_3_0_GLRENDERLOOPER_H

#include "Looper.h"
#include <GLES3/gl3.h>
#include <thread>
#include <EGL/egl.h>
#include <LogUtil.h>
#include <EglCore.h>
#include <OffscreenSurface.h>
#include <ImageDef.h>

using namespace std;

// 消息类型枚举
enum {
    MSG_SurfaceCreated,   // Surface 创建消息
    MSG_SurfaceChanged,   // Surface 尺寸改变消息
    MSG_DrawFrame,        // 绘制帧消息
    MSG_SurfaceDestroyed, // Surface 销毁消息
};

// 渲染完成的回调函数类型定义
typedef void (*RenderDoneCallback)(void*, int);

// GL 环境结构体，包含相关的 OpenGL 和 EGL 参数
struct GLEnv {
    GLuint inputTexId;          // 输入纹理 ID
    GLuint program;             // 渲染程序 ID
    GLuint vboIds[3];           // 顶点缓冲对象 ID 数组
    EGLContext sharedCtx;       // 共享的 EGL 上下文
    SizeF imgSize;              // 图像尺寸
    RenderDoneCallback renderDone; // 渲染完成回调
    void* callbackCtx;          // 回调上下文
};

// GLRenderLooper 类，用于管理 OpenGL 渲染循环
class GLRenderLooper : public Looper {
public:
    // 构造函数和析构函数
    GLRenderLooper();
    virtual ~GLRenderLooper();

    // 获取单例实例
    static GLRenderLooper* GetInstance();

    // 释放单例实例
    static void ReleaseInstance();

private:
    // 处理消息的虚函数
    virtual void handleMessage(LooperMessage *msg);

    // Surface 相关操作
    void OnSurfaceCreated(); // 处理 Surface 创建
    void OnSurfaceChanged(int w, int h); // 处理 Surface 尺寸改变
    void OnDrawFrame(); // 绘制帧
    void OnSurfaceDestroyed(); // 处理 Surface 销毁

    // 创建帧缓冲对象 (FBO)
    bool CreateFrameBufferObj();

private:
    static mutex m_Mutex;               // 线程安全的互斥锁
    static GLRenderLooper* m_Instance; // 单例实例

    GLEnv *m_GLEnv;                     // GL 环境指针
    EglCore *m_EglCore = nullptr;       // EGL 环境核心
    OffscreenSurface *m_OffscreenSurface = nullptr; // 离屏 Surface
    GLuint m_VaoId;                     // 顶点数组对象 (VAO) ID
    GLuint m_FboTextureId;              // 帧缓冲纹理 ID
    GLuint m_FboId;                     // 帧缓冲对象 ID
    int m_FrameIndex = 0;               // 帧计数器
};

#endif //NDK_OPENGLES_3_0_GLRENDERLOOPER_H
