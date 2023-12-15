#ifndef NDK_OPENGLES_3_0_BASIC_LIGHTING_H
#define NDK_OPENGLES_3_0_BASIC_LIGHTING_H

#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include "GLSampleBase.h"

/**
 * @class BasicLightingSample
 * @brief 一个用于展示基础光照效果的 OpenGL 示例类。
 *
 * 该类继承自 GLSampleBase，包含初始化、加载资源、渲染和销毁逻辑。实现了环境光、漫反射和高光的光照计算，
 * 并支持加载纹理和模型矩阵变换。
 */
class BasicLightingSample : public GLSampleBase
{
public:
    /**
     * @brief 构造函数，初始化成员变量。
     */
    BasicLightingSample();

    /**
     * @brief 析构函数，释放资源。
     */
    virtual ~BasicLightingSample();

    /**
     * @brief 加载纹理图像。
     * @param pImage 指向包含图像数据的 NativeImage 结构体的指针。
     */
    virtual void LoadImage(NativeImage *pImage);

    /**
     * @brief 初始化 OpenGL 相关资源和着色器。
     */
    virtual void Init();

    /**
     * @brief 渲染场景。
     * @param screenW 屏幕宽度。
     * @param screenH 屏幕高度。
     */
    virtual void Draw(int screenW, int screenH);

    /**
     * @brief 销毁 OpenGL 资源。
     */
    virtual void Destroy();

    /**
     * @brief 更新模型的变换矩阵。
     * @param rotateX 绕 X 轴的旋转角度。
     * @param rotateY 绕 Y 轴的旋转角度。
     * @param scaleX X 轴的缩放比例。
     * @param scaleY Y 轴的缩放比例。
     */
    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    /**
     * @brief 更新 MVP 矩阵（模型-视图-投影矩阵）。
     * @param mvpMatrix 输出的 MVP 矩阵。
     * @param angleX 绕 X 轴的旋转角度。
     * @param angleY 绕 Y 轴的旋转角度。
     * @param ratio 屏幕宽高比。
     */
    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    // 纹理 ID
    GLuint m_TextureId;
    // 着色器中纹理采样器的位置
    GLint m_SamplerLoc;
    // 着色器中 MVP 矩阵的位置
    GLint m_MVPMatLoc;
    // 着色器中模型矩阵的位置
    GLint m_ModelMatrixLoc;
    // 着色器中光源位置的位置
    GLint m_LightPosLoc;
    // 着色器中光源颜色的位置
    GLint m_LightColorLoc;
    // 着色器中视点位置的位置
    GLint m_ViewPosLoc;
    // 顶点数组对象 ID
    GLuint m_VaoId;
    // 顶点缓冲对象 ID 数组
    GLuint m_VboIds[1];
    // Transform Feedback 对象 ID
    GLuint m_TfoId;
    // Transform Feedback 缓冲对象 ID
    GLuint m_TfoBufId;
    // 存储渲染所需的图像数据
    NativeImage m_RenderImage;
    // MVP 矩阵
    glm::mat4 m_MVPMatrix;
    // 模型矩阵
    glm::mat4 m_ModelMatrix;
    // 绕 X 轴的旋转角度
    int m_AngleX;
    // 绕 Y 轴的旋转角度
    int m_AngleY;
    // X 轴的缩放比例
    float m_ScaleX;
    // Y 轴的缩放比例
    float m_ScaleY;
};

#endif //NDK_OPENGLES_3_0_BASIC_LIGHTING_H
