package com.byteflow.app

import android.opengl.GLSurfaceView
import android.util.Log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

/**
 * MyGLRender 类是一个实现 GLSurfaceView.Renderer 接口的渲染器类，
 * 用于管理 OpenGL 渲染的生命周期方法，并调用本地渲染器 MyNativeRender 执行具体渲染逻辑。
 */
class MyGLRender internal constructor() : GLSurfaceView.Renderer {
    private val mNativeRender: MyNativeRender = MyNativeRender() // 本地渲染器对象
    var sampleType = 0 // 当前渲染的样例类型
        private set

    /**
     * 当渲染表面创建时调用，初始化 OpenGL 相关资源。
     *
     * @param gl OpenGL 接口。
     * @param config EGL 配置。
     */
    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        mNativeRender.native_OnSurfaceCreated() // 调用本地方法初始化资源
        Log.e(
            TAG,
            "onSurfaceCreated() called with: GL_VERSION = [" + gl.glGetString(GL10.GL_VERSION) + "]"
        )
    }

    /**
     * 当渲染表面大小发生变化时调用，更新视口的宽高。
     *
     * @param gl OpenGL 接口。
     * @param width 渲染表面的宽度。
     * @param height 渲染表面的高度。
     */
    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        mNativeRender.native_OnSurfaceChanged(width, height) // 调用本地方法更新视口尺寸
    }

    /**
     * 当需要绘制一帧时调用，执行渲染逻辑。
     *
     * @param gl OpenGL 接口。
     */
    override fun onDrawFrame(gl: GL10) {
        mNativeRender.native_OnDrawFrame() // 调用本地方法进行绘制
    }

    /**
     * 初始化渲染器。
     * 调用本地方法完成渲染器的初始化操作。
     */
    fun init() {
        mNativeRender.native_Init()
    }

    /**
     * 释放渲染器资源。
     * 调用本地方法清理渲染器占用的资源。
     */
    fun unInit() {
        mNativeRender.native_UnInit()
    }

    /**
     * 设置整型参数，用于控制渲染逻辑。
     *
     * @param paramType 参数类型。
     * @param value0 参数值 0。
     * @param value1 参数值 1。
     */
    fun setParamsInt(paramType: Int, value0: Int, value1: Int) {
        if (paramType == MyNativeRender.SAMPLE_TYPE) {
            sampleType = value0 // 更新当前样例类型
        }
        mNativeRender.native_SetParamsInt(paramType, value0, value1)
    }

    /**
     * 设置触摸点位置。
     *
     * @param x 触摸点的 X 坐标。
     * @param y 触摸点的 Y 坐标。
     */
    fun setTouchLoc(x: Float, y: Float) {
        mNativeRender.native_SetParamsFloat(MyNativeRender.SAMPLE_TYPE_SET_TOUCH_LOC, x, y)
    }

    /**
     * 设置重力传感器的 X 和 Y 数据。
     *
     * @param x 重力传感器的 X 数据。
     * @param y 重力传感器的 Y 数据。
     */
    fun setGravityXY(x: Float, y: Float) {
        mNativeRender.native_SetParamsFloat(MyNativeRender.SAMPLE_TYPE_SET_GRAVITY_XY, x, y)
    }

    /**
     * 设置图像数据，并传递给本地渲染器。
     *
     * @param format 图像格式。
     * @param width 图像宽度。
     * @param height 图像高度。
     * @param bytes 图像数据的字节数组。
     */
    fun setImageData(format: Int, width: Int, height: Int, bytes: ByteArray?) {
        mNativeRender.native_SetImageData(format, width, height, bytes)
    }

    /**
     * 设置带索引的图像数据，并传递给本地渲染器。
     *
     * @param index 图像的索引。
     * @param format 图像格式。
     * @param width 图像宽度。
     * @param height 图像高度。
     * @param bytes 图像数据的字节数组。
     */
    fun setImageDataWithIndex(index: Int, format: Int, width: Int, height: Int, bytes: ByteArray?) {
        mNativeRender.native_SetImageDataWithIndex(index, format, width, height, bytes)
    }

    /**
     * 设置音频数据，并传递给本地渲染器。
     *
     * @param audioData 音频数据的短整型数组。
     */
    fun setAudioData(audioData: ShortArray?) {
        mNativeRender.native_SetAudioData(audioData)
    }

    /**
     * 更新变换矩阵，控制旋转和缩放。
     *
     * @param rotateX X 轴的旋转角度。
     * @param rotateY Y 轴的旋转角度。
     * @param scaleX X 轴的缩放比例。
     * @param scaleY Y 轴的缩放比例。
     */
    fun updateTransformMatrix(rotateX: Float, rotateY: Float, scaleX: Float, scaleY: Float) {
        mNativeRender.native_UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY)
    }

    companion object {
        private const val TAG = "MyGLRender" // 日志标记
    }
}