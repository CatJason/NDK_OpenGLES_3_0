//
// Created by Administrator on 2018/3/21.
//

#include <assert.h>
#include <GLES2/gl2.h>
#include <LogUtil.h>
#include "EglSurfaceBase.h"

EglSurfaceBase::EglSurfaceBase(EglCore *eglCore) : mEglCore(eglCore) {
    mEglSurface = EGL_NO_SURFACE; // 初始化 EGLSurface
}

/**
 * 创建显示的Surface
 * @param nativeWindow 本地窗口指针，用于创建 EGL 显示 Surface
 */
void EglSurfaceBase::createWindowSurface(ANativeWindow *nativeWindow) {
    assert(mEglSurface == EGL_NO_SURFACE); // 确保 Surface 尚未创建
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGCATE("surface already created\n"); // 防止重复创建
        return;
    }
    mEglSurface = mEglCore->createWindowSurface(nativeWindow); // 使用 EGLCore 创建窗口 Surface
}

/**
 * 创建离屏Surface
 * @param width 离屏 Surface 的宽度
 * @param height 离屏 Surface 的高度
 */
void EglSurfaceBase::createOffscreenSurface(int width, int height) {
    assert(mEglSurface == EGL_NO_SURFACE); // 确保 Surface 尚未创建
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGCATE("surface already created\n"); // 防止重复创建
        return;
    }
    mEglSurface = mEglCore->createOffscreenSurface(width, height); // 使用 EGLCore 创建离屏 Surface
    mWidth = width;  // 设置宽度
    mHeight = height; // 设置高度
}

/**
 * 获取 Surface 的宽度
 * @return 当前 Surface 的宽度
 */
int EglSurfaceBase::getWidth() {
    if (mWidth < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_WIDTH); // 查询宽度
    } else {
        return mWidth;
    }
}

/**
 * 获取 Surface 的高度
 * @return 当前 Surface 的高度
 */
int EglSurfaceBase::getHeight() {
    if (mHeight < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_HEIGHT); // 查询高度
    } else {
        return mHeight;
    }
}

/**
 * 释放 EGLSurface 资源
 */
void EglSurfaceBase::releaseEglSurface() {
    mEglCore->releaseSurface(mEglSurface); // 释放 EGLSurface
    mEglSurface = EGL_NO_SURFACE; // 重置 Surface
    mWidth = mHeight = -1; // 重置宽度和高度
}

/**
 * 切换到当前 EGL 上下文
 */
void EglSurfaceBase::makeCurrent() {
    mEglCore->makeCurrent(mEglSurface); // 切换到当前 Surface 的上下文
}

/**
 * 将渲染结果交换到前台显示
 * @return 是否成功交换缓冲区
 */
bool EglSurfaceBase::swapBuffers() {
    bool result = mEglCore->swapBuffers(mEglSurface); // 交换缓冲区
    if (!result) {
        LOGCATD("WARNING: swapBuffers() failed"); // 记录失败信息
    }
    return result;
}

/**
 * 设置当前 Surface 的时间戳
 * @param nsecs 时间戳，单位为纳秒
 */
void EglSurfaceBase::setPresentationTime(long nsecs) {
    mEglCore->setPresentationTime(mEglSurface, nsecs); // 设置时间戳
}

/**
 * 获取当前 Surface 的像素数据
 * @return 包含像素数据的指针
 */
char* EglSurfaceBase::getCurrentFrame() {
    char *pixels = NULL;
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels); // 读取像素数据
    return pixels;
}