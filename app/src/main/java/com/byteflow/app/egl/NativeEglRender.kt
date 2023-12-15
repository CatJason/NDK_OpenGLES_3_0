package com.byteflow.app.egl

class NativeEglRender {
    external fun native_EglRenderInit()
    external fun native_EglRenderSetImageData(data: ByteArray?, width: Int, height: Int)
    external fun native_EglRenderSetIntParams(paramType: Int, param: Int)
    external fun native_EglRenderDraw()
    external fun native_EglRenderUnInit()
}