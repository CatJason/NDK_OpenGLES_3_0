//
// Created by Administrator on 2018/3/21.
//

#include <assert.h>
#include <LogUtil.h>
#include "WindowSurface.h"

/**
 * 带释放选项的窗口Surface构造函数
 * @param eglCore EglCore对象，用于管理EGL环境
 * @param window ANativeWindow对象，表示本地窗口
 * @param releaseSurface 指定是否在释放时销毁ANativeWindow
 */
WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface)
        : EglSurfaceBase(eglCore) {
    mSurface = window; // 保存窗口对象
    createWindowSurface(mSurface); // 创建窗口类型的EGLSurface
    mReleaseSurface = releaseSurface; // 保存释放标志
}

/**
 * 默认的窗口Surface构造函数
 * @param eglCore EglCore对象，用于管理EGL环境
 * @param window ANativeWindow对象，表示本地窗口
 */
WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window)
        : EglSurfaceBase(eglCore) {
    createWindowSurface(window); // 创建窗口类型的EGLSurface
    mSurface = window; // 保存窗口对象
}

/**
 * 释放窗口Surface资源
 * 包括释放EGLSurface和ANativeWindow
 */
void WindowSurface::release() {
    releaseEglSurface(); // 释放EGLSurface资源
    if (mSurface != NULL) {
        ANativeWindow_release(mSurface); // 释放本地窗口资源
        mSurface = NULL; // 重置窗口对象
    }
}

/**
 * 重新创建窗口Surface
 * @param eglCore 新的EglCore对象，用于管理EGL环境
 */
void WindowSurface::recreate(EglCore *eglCore) {
    assert(mSurface != NULL); // 确保窗口对象不为空
    if (mSurface == NULL) {
        LOGCATE("not yet implemented ANativeWindow"); // 如果窗口对象为空，记录日志
        return;
    }
    mEglCore = eglCore; // 更新EglCore对象
    createWindowSurface(mSurface); // 使用新的EglCore重新创建EGLSurface
}