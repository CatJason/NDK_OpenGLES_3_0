//
// Created by Administrator on 2018/3/21.
//

#ifndef EGL_HELPER_WINDOWSURFACE_H
#define EGL_HELPER_WINDOWSURFACE_H

#include <android/native_window.h>
#include "EglSurfaceBase.h"

/**
 * WindowSurface 类
 * 用于管理基于窗口的 EGLSurface
 */
class WindowSurface : public EglSurfaceBase {

public:
    /**
     * 带释放选项的构造函数
     * @param eglCore EglCore对象，用于管理EGL环境
     * @param window 本地窗口（ANativeWindow）对象
     * @param releaseSurface 是否在释放时销毁ANativeWindow
     */
    WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface);

    /**
     * 默认构造函数
     * @param eglCore EglCore对象，用于管理EGL环境
     * @param window 本地窗口（ANativeWindow）对象
     */
    WindowSurface(EglCore *eglCore, ANativeWindow *window);

    /**
     * 释放资源
     * 包括EGLSurface和ANativeWindow
     */
    void release();

    /**
     * 重新创建EGLSurface
     * @param eglCore 新的EglCore对象
     */
    void recreate(EglCore *eglCore);

private:
    ANativeWindow  *mSurface; ///< 本地窗口对象
    bool mReleaseSurface; ///< 是否在释放时销毁窗口的标志
};

#endif //EGL_HELPER_WINDOWSURFACE_H
