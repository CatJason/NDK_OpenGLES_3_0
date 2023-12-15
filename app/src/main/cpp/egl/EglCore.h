//
// Created by Administrator on 2018/2/10.
//

#ifndef EGL_HELPER_EGLCORE_H
#define EGL_HELPER_EGLCORE_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

/**
 * 构造标志：Surface 必须是可录制的。这会阻止 EGL 使用无法有效转换为视频编码器可用像素格式的配置。
 */
#define FLAG_RECORDABLE 0x01

/**
 * 构造标志：请求使用 GLES3，如果不可用则回退到 GLES2。若未设置此标志，则默认使用 GLES2。
 */
#define FLAG_TRY_GLES3 002

// Android 特定扩展
#define EGL_RECORDABLE_ANDROID 0x3142

typedef EGLBoolean (EGLAPIENTRYP EGL_PRESENTATION_TIME_ANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);

class EglCore {

private:
    EGLDisplay mEGLDisplay = EGL_NO_DISPLAY; // EGL 显示
    EGLConfig  mEGLConfig = NULL;            // EGL 配置
    EGLContext mEGLContext = EGL_NO_CONTEXT; // EGL 上下文

    // 设置时间戳的方法
    EGL_PRESENTATION_TIME_ANDROIDPROC eglPresentationTimeANDROID = NULL;

    int mGlVersion = -1; // OpenGL ES 版本号
    // 查找合适的 EGLConfig
    EGLConfig getConfig(int flags, int version);

public:
    EglCore();
    ~EglCore();
    EglCore(EGLContext sharedContext, int flags);
    bool init(EGLContext sharedContext, int flags); // 初始化
    // 释放资源
    void release();
    // 获取 EGL 上下文
    EGLContext getEGLContext();
    // 销毁 Surface
    void releaseSurface(EGLSurface eglSurface);
    // 创建窗口类型的 EGLSurface
    EGLSurface createWindowSurface(ANativeWindow *surface);
    // 创建离屏类型的 EGLSurface
    EGLSurface createOffscreenSurface(int width, int height);
    // 切换到当前上下文
    void makeCurrent(EGLSurface eglSurface);
    // 切换到指定上下文
    void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface);
    // 清除上下文
    void makeNothingCurrent();
    // 交换显示缓冲区
    bool swapBuffers(EGLSurface eglSurface);
    // 设置时间戳
    void setPresentationTime(EGLSurface eglSurface, long nsecs);
    // 检查是否为当前上下文
    bool isCurrent(EGLSurface eglSurface);
    // 查询 Surface 参数
    int querySurface(EGLSurface eglSurface, int what);
    // 查询字符串
    const char *queryString(int what);
    // 获取当前的 OpenGL ES 版本号
    int getGlVersion();
    // 检查 EGL 是否出错
    void checkEglError(const char *msg);
};

#endif //EGL_HELPER_EGLCORE_H
