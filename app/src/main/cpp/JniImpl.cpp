//
// Created by ByteFlow on 2019/7/9.
//
#include "util/LogUtil.h"
#include <MyGLRenderContext.h>
#include <EGLRender.h>
#include "jni.h"

#define NATIVE_RENDER_CLASS_NAME "com/byteflow/app/MyNativeRender"
#define NATIVE_BG_RENDER_CLASS_NAME "com/byteflow/app/egl/NativeEglRender"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_Init
 * 签名:     ()V
 * 功能:     初始化 MyGLRenderContext 单例对象。
 */
JNIEXPORT void JNICALL native_Init(JNIEnv *env, jobject instance)
{
    MyGLRenderContext::GetInstance();
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_UnInit
 * 签名:     ()V
 * 功能:     销毁 MyGLRenderContext 单例对象。
 */
JNIEXPORT void JNICALL native_UnInit(JNIEnv *env, jobject instance)
{
    MyGLRenderContext::DestroyInstance();
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_SetImageData
 * 签名:     (III[B)V
 * 功能:     设置图像数据，包括格式、宽度、高度及图像字节数组。
 */
JNIEXPORT void JNICALL native_SetImageData
        (JNIEnv *env, jobject instance, jint format, jint width, jint height, jbyteArray imageData)
{
    int len = env->GetArrayLength(imageData);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
    MyGLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_SetImageDataWithIndex
 * 签名:     (IIII[B)V
 * 功能:     设置带索引的图像数据，包括索引值、格式、宽度、高度及图像字节数组。
 */
JNIEXPORT void JNICALL native_SetImageDataWithIndex
        (JNIEnv *env, jobject instance, jint index, jint format, jint width, jint height, jbyteArray imageData)
{
    int len = env->GetArrayLength(imageData);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
    MyGLRenderContext::GetInstance()->SetImageDataWithIndex(index, format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_SetParamsInt
 * 签名:     (III)V
 * 功能:     设置整型参数，用于配置 MyGLRenderContext。
 */
JNIEXPORT void JNICALL native_SetParamsInt
        (JNIEnv *env, jobject instance, jint paramType, jint value0, jint value1)
{
    MyGLRenderContext::GetInstance()->SetParamsInt(paramType, value0, value1);
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_SetParamsFloat
 * 签名:     (IFF)V
 * 功能:     设置浮点型参数，用于配置 MyGLRenderContext。
 */
JNIEXPORT void JNICALL native_SetParamsFloat
        (JNIEnv *env, jobject instance, jint paramType, jfloat value0, jfloat value1)
{
    MyGLRenderContext::GetInstance()->SetParamsFloat(paramType, value0, value1);
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_SetAudioData
 * 签名:     ([B)V
 * 功能:     设置音频数据，接收 Java 层的短整型数组并传递给 MyGLRenderContext。
 */
JNIEXPORT void JNICALL native_SetAudioData
        (JNIEnv *env, jobject instance, jshortArray data)
{
    int len = env->GetArrayLength(data);
    short *pShortBuf = new short[len];
    env->GetShortArrayRegion(data, 0, len, reinterpret_cast<jshort*>(pShortBuf));
    MyGLRenderContext::GetInstance()->SetParamsShortArr(pShortBuf, len);
    delete[] pShortBuf;
    env->DeleteLocalRef(data);
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_UpdateTransformMatrix
 * 签名:     (FFFF)V
 * 功能:     更新变换矩阵，接收旋转和缩放参数。
 */
JNIEXPORT void JNICALL native_UpdateTransformMatrix(JNIEnv *env, jobject instance, jfloat rotateX, jfloat rotateY, jfloat scaleX, jfloat scaleY)
{
    MyGLRenderContext::GetInstance()->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_OnSurfaceCreated
 * 签名:     ()V
 * 功能:     当渲染表面创建时调用，通知 MyGLRenderContext 初始化资源。
 */
JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance)
{
    MyGLRenderContext::GetInstance()->OnSurfaceCreated();
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_OnSurfaceChanged
 * 签名:     (II)V
 * 功能:     当渲染表面尺寸改变时调用，更新宽度和高度。
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height)
{
    MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
}

/**
 * 类名:     com_byteflow_app_MyNativeRender
 * 方法:     native_OnDrawFrame
 * 签名:     ()V
 * 功能:     绘制一帧，通过调用 MyGLRenderContext 的 OnDrawFrame 方法完成渲染。
 */
JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance)
{
    MyGLRenderContext::GetInstance()->OnDrawFrame();
}

/**
 * 类名:     com_byteflow_app_egl_NativeBgRender
 * 方法:     native_EglRenderInit
 * 签名:     ()V
 * 功能:     初始化 EGL 渲染器，通过调用 EGLRender 的 Init 方法。
 */
JNIEXPORT void JNICALL native_EglRenderInit(JNIEnv *env, jobject instance)
{
    EGLRender::GetInstance()->Init();
}

/**
 * 类名:     com_byteflow_app_egl_NativeBgRender
 * 方法:     native_EglRenderSetImageData
 * 签名:     ([BII)V
 * 功能:     设置渲染图像数据，接收宽度、高度及图像字节数组。
 */
JNIEXPORT void JNICALL native_EglRenderSetImageData(JNIEnv *env, jobject instance, jbyteArray data, jint width, jint height)
{
    int len = env->GetArrayLength(data);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
    EGLRender::GetInstance()->SetImageData(buf, width, height);
    delete[] buf;
    env->DeleteLocalRef(data);
}

/**
 * 类名:     com_byteflow_app_egl_NativeBgRender
 * 方法:     native_EglRenderSetIntParams
 * 签名:     (II)V
 * 功能:     设置整型参数，用于配置 EGL 渲染器。
 */
JNIEXPORT void JNICALL native_EglRenderSetIntParams(JNIEnv *env, jobject instance, jint type, jint param)
{
    EGLRender::GetInstance()->SetIntParams(type, param);
}

/**
 * 类名:     com_byteflow_app_egl_NativeBgRender
 * 方法:     native_EglRenderDraw
 * 签名:     ()V
 * 功能:     执行渲染操作，通过调用 EGLRender 的 Draw 方法完成渲染。
 */
JNIEXPORT void JNICALL native_EglRenderDraw(JNIEnv *env, jobject instance)
{
    EGLRender::GetInstance()->Draw();
}

/**
 * 类名:     com_byteflow_app_egl_NativeBgRender
 * 方法:     natuve_BgRenderUnInit
 * 签名:     ()V
 * 功能:     释放 EGL 渲染器资源，通过调用 EGLRender 的 UnInit 方法。
 */
JNIEXPORT void JNICALL natuve_BgRenderUnInit(JNIEnv *env, jobject instance)
{
    EGLRender::GetInstance()->UnInit();
}

#ifdef __cplusplus
}
#endif

static JNINativeMethod g_RenderMethods[] = {
		{"native_Init",                      "()V",       (void *)(native_Init)},
		{"native_UnInit",                    "()V",       (void *)(native_UnInit)},
		{"native_SetImageData",              "(III[B)V",  (void *)(native_SetImageData)},
		{"native_SetImageDataWithIndex",     "(IIII[B)V", (void *)(native_SetImageDataWithIndex)},
		{"native_SetParamsInt",              "(III)V",    (void *)(native_SetParamsInt)},
		{"native_SetParamsFloat",            "(IFF)V",    (void *)(native_SetParamsFloat)},
		{"native_SetAudioData",              "([S)V",     (void *)(native_SetAudioData)},
		{"native_UpdateTransformMatrix",     "(FFFF)V",   (void *)(native_UpdateTransformMatrix)},
		{"native_OnSurfaceCreated",          "()V",       (void *)(native_OnSurfaceCreated)},
		{"native_OnSurfaceChanged",          "(II)V",     (void *)(native_OnSurfaceChanged)},
		{"native_OnDrawFrame",               "()V",       (void *)(native_OnDrawFrame)},
};

static JNINativeMethod g_BgRenderMethods[] = {
		{"native_EglRenderInit",          "()V",       (void *)(native_EglRenderInit)},
		{"native_EglRenderSetImageData",  "([BII)V",   (void *)(native_EglRenderSetImageData)},
		{"native_EglRenderSetIntParams",  "(II)V",     (void *)(native_EglRenderSetIntParams)},
		{"native_EglRenderDraw",          "()V",       (void *)(native_EglRenderDraw)},
		{"native_EglRenderUnInit",        "()V",       (void *)(natuve_BgRenderUnInit)},
};

/**
 * 方法:     RegisterNativeMethods
 * 参数:     env - JNI 环境
 *           className - 类名字符串
 *           methods - 方法表指针
 *           methodNum - 方法数量
 * 返回:     JNI_TRUE 表示注册成功，JNI_FALSE 表示失败
 * 功能:     将本地方法注册到指定的 Java 类中。
 */
static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
    LOGCATE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0)
    {
        LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/**
 * 方法:     UnregisterNativeMethods
 * 参数:     env - JNI 环境
 *           className - 类名字符串
 * 功能:     从指定的 Java 类中解除本地方法的注册。
 */
static void UnregisterNativeMethods(JNIEnv *env, const char *className)
{
    LOGCATE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL)
    {
        env->UnregisterNatives(clazz);
    }
}

/**
 * 方法:     JNI_OnLoad
 * 参数:     jvm - Java 虚拟机实例
 *           p - 附加参数
 * 返回:     JNI 版本号
 * 功能:     动态库加载时调用，注册本地方法并进行必要的初始化。
 */
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p)
{
    LOGCATE("===== JNI_OnLoad =====");
    jint jniRet = JNI_ERR;
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return jniRet;
    }

    // 注册 MyNativeRender 类中的本地方法
    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_RenderMethods,
                                        sizeof(g_RenderMethods) /
                                        sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE)
    {
        return JNI_ERR;
    }

    // 注册 NativeEglRender 类中的本地方法
    regRet = RegisterNativeMethods(env, NATIVE_BG_RENDER_CLASS_NAME, g_BgRenderMethods,
                                   sizeof(g_BgRenderMethods) /
                                   sizeof(g_BgRenderMethods[0]));
    if (regRet != JNI_TRUE)
    {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

/**
 * 方法:     JNI_OnUnload
 * 参数:     jvm - Java 虚拟机实例
 *           p - 附加参数
 * 功能:     动态库卸载时调用，解除本地方法注册并释放资源。
 */
extern "C" void JNI_OnUnload(JavaVM *jvm, void *p)
{
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return;
    }

    // 解除 MyNativeRender 类中的本地方法注册
    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);

    // 解除 NativeEglRender 类中的本地方法注册
    UnregisterNativeMethods(env, NATIVE_BG_RENDER_CLASS_NAME);
}