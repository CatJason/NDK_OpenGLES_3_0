package com.byteflow.app

import android.opengl.GLSurfaceView
import android.util.Log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyGLRender internal constructor() : GLSurfaceView.Renderer {
    private val mNativeRender: MyNativeRender = MyNativeRender()
    var sampleType = 0
        private set

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        mNativeRender.native_OnSurfaceCreated()
        Log.e(
            TAG,
            "onSurfaceCreated() called with: GL_VERSION = [" + gl.glGetString(GL10.GL_VERSION) + "]"
        )
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        mNativeRender.native_OnSurfaceChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        mNativeRender.native_OnDrawFrame()
    }

    fun init() {
        mNativeRender.native_Init()
    }

    fun unInit() {
        mNativeRender.native_UnInit()
    }

    fun setParamsInt(paramType: Int, value0: Int, value1: Int) {
        if (paramType == MyNativeRender.SAMPLE_TYPE) {
            sampleType = value0
        }
        mNativeRender.native_SetParamsInt(paramType, value0, value1)
    }

    fun setTouchLoc(x: Float, y: Float) {
        mNativeRender.native_SetParamsFloat(MyNativeRender.SAMPLE_TYPE_SET_TOUCH_LOC, x, y)
    }

    fun setGravityXY(x: Float, y: Float) {
        mNativeRender.native_SetParamsFloat(MyNativeRender.SAMPLE_TYPE_SET_GRAVITY_XY, x, y)
    }

    fun setImageData(format: Int, width: Int, height: Int, bytes: ByteArray?) {
        mNativeRender.native_SetImageData(format, width, height, bytes)
    }

    fun setImageDataWithIndex(index: Int, format: Int, width: Int, height: Int, bytes: ByteArray?) {
        mNativeRender.native_SetImageDataWithIndex(index, format, width, height, bytes)
    }

    fun setAudioData(audioData: ShortArray?) {
        mNativeRender.native_SetAudioData(audioData)
    }

    fun updateTransformMatrix(rotateX: Float, rotateY: Float, scaleX: Float, scaleY: Float) {
        mNativeRender.native_UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY)
    }

    companion object {
        private const val TAG = "MyGLRender"
    }
}