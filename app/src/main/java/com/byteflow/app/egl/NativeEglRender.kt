package com.byteflow.app.egl

/**
 * 本地 EGL 渲染器类，定义与本地 C/C++ 层交互的接口。
 * 提供 EGL 渲染的初始化、图像数据设置、参数设置、绘制和释放等功能。
 */
class NativeEglRender {

    /**
     * 初始化 EGL 渲染器。
     * 在本地层中完成 EGL 环境的创建和初始化。
     */
    external fun native_EglRenderInit()

    /**
     * 设置 RGBA 格式的图像数据。
     * @param data 图像数据的字节数组（RGBA 格式）。
     * @param width 图像宽度。
     * @param height 图像高度。
     */
    external fun native_EglRenderSetImageData(data: ByteArray?, width: Int, height: Int)

    /**
     * 设置整型参数，用于配置渲染器。
     * @param paramType 参数类型。
     * @param param 参数值。
     */
    external fun native_EglRenderSetIntParams(paramType: Int, param: Int)

    /**
     * 执行绘制操作。
     * 在本地层调用 EGL 的绘制方法，将内容渲染到屏幕或纹理上。
     */
    external fun native_EglRenderDraw()

    /**
     * 释放 EGL 渲染器资源。
     * 在本地层中销毁 EGL 环境并释放相关资源。
     */
    external fun native_EglRenderUnInit()
}