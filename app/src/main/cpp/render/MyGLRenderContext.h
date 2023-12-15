//
// Created by ByteFlow on 2019/7/9.
//

#ifndef NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H
#define NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H

#include "stdint.h"
#include <GLES3/gl3.h>
#include "TextureMapSample.h"
#include "NV21TextureMapSample.h"
#include "TriangleSample.h"

/**
 * @brief OpenGL 渲染上下文管理器类
 *
 * 该类管理 OpenGL 的渲染生命周期，包括初始化、绘制帧、加载数据、设置参数等功能。
 * 它使用单例模式确保全局只有一个渲染上下文实例。
 */
class MyGLRenderContext
{
    /**
     * @brief 私有构造函数，禁止外部直接实例化
     */
    MyGLRenderContext();

    /**
     * @brief 析构函数，释放资源
     */
    ~MyGLRenderContext();

public:
    /**
     * @brief 设置图像数据
     *
     * @param format 图像格式（如 NV21、I420 等）
     * @param width 图像宽度
     * @param height 图像高度
     * @param pData 图像数据指针
     */
    void SetImageData(int format, int width, int height, uint8_t *pData);

    /**
     * @brief 设置多图像数据，通过索引区分不同图像
     *
     * @param index 图像索引
     * @param format 图像格式
     * @param width 图像宽度
     * @param height 图像高度
     * @param pData 图像数据指针
     */
    void SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData);

    /**
     * @brief 设置整型参数
     *
     * @param paramType 参数类型（如 SAMPLE_TYPE）
     * @param value0 参数值 0
     * @param value1 参数值 1（备用）
     */
    void SetParamsInt(int paramType, int value0, int value1);

    /**
     * @brief 设置浮点型参数
     *
     * @param paramType 参数类型
     * @param value0 参数值 0
     * @param value1 参数值 1
     */
    void SetParamsFloat(int paramType, float value0, float value1);

    /**
     * @brief 设置短整型数组参数
     *
     * @param pShortArr 短整型数组指针
     * @param arrSize 数组大小
     */
    void SetParamsShortArr(short *const pShortArr, int arrSize);

    /**
     * @brief 更新变换矩阵
     *
     * @param rotateX 绕 X 轴旋转角度
     * @param rotateY 绕 Y 轴旋转角度
     * @param scaleX X 轴缩放比例
     * @param scaleY Y 轴缩放比例
     */
    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    /**
     * @brief OpenGL 表面创建回调
     *
     * 初始化背景色等设置。
     */
    void OnSurfaceCreated();

    /**
     * @brief OpenGL 表面大小变化回调
     *
     * @param width 新的宽度
     * @param height 新的高度
     */
    void OnSurfaceChanged(int width, int height);

    /**
     * @brief 绘制一帧
     *
     * 清除缓冲区并调用当前样例的绘制方法。
     */
    void OnDrawFrame();

    /**
     * @brief 获取单例实例
     *
     * @return MyGLRenderContext* 单例对象指针
     */
    static MyGLRenderContext* GetInstance();

    /**
     * @brief 销毁单例实例
     */
    static void DestroyInstance();

private:
    static MyGLRenderContext *m_pContext; ///< 静态单例指针
    GLSampleBase *m_pBeforeSample; ///< 上一个渲染样例
    GLSampleBase *m_pCurSample; ///< 当前渲染样例
    int m_ScreenW; ///< 屏幕宽度
    int m_ScreenH; ///< 屏幕高度
};

#endif //NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H