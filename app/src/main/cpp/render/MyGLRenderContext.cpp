//
// Created by ByteFlow on 2019/7/9.
//

#include <TriangleSample.h>
#include <TextureMapSample.h>
#include <NV21TextureMapSample.h>
#include <VaoSample.h>
#include <FBOSample.h>
#include <FBOLegLengthenSample.h>
#include <CoordSystemSample.h>
#include <BasicLightingSample.h>
#include <TransformFeedbackSample.h>
#include <MultiLightsSample.h>
#include <DepthTestingSample.h>
#include <InstancingSample.h>
#include <Instancing3DSample.h>
#include <StencilTestingSample.h>
#include <BlendingSample.h>
#include <ParticlesSample.h>
#include <Noise3DSample.h>
#include <SkyBoxSample.h>
#include <Model3DSample.h>
#include <PBOSample.h>
#include <BeatingHeartSample.h>
#include <CloudSample.h>
#include <TimeTunnelSample.h>
#include <BezierCurveSample.h>
#include <BigEyesSample.h>
#include <FaceSlenderSample.h>
#include <BigHeadSample.h>
#include <RotaryHeadSample.h>
#include <VisualizeAudioSample.h>
#include <ScratchCardSample.h>
#include <AvatarSample.h>
#include <ShockWaveSample.h>
#include <MRTSample.h>
#include <FBOBlitSample.h>
#include <TextureBufferSample.h>
#include <UniformBufferSample.h>
#include <RGB2YUYVSample.h>
#include <SharedEGLContextSample.h>
#include <TextRenderSample.h>
#include <PortraitStayColorExample.h>
#include <GLTransitionExample.h>
#include <GLTransitionExample_2.h>
#include <GLTransitionExample_3.h>
#include <GLTransitionExample_4.h>
#include <ConveyorBeltExample.h>
#include <RGB2NV21Sample.h>
#include <RGB2I420Sample.h>
#include <RGB2I444Sample.h>
#include <CopyTextureExample.h>
#include <BlitFrameBufferExample.h>
#include <BinaryProgramExample.h>
#include "MyGLRenderContext.h"
#include "LogUtil.h"

MyGLRenderContext* MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext()
{
	m_pCurSample = new BeatingHeartSample();
	m_pBeforeSample = nullptr;

}

MyGLRenderContext::~MyGLRenderContext()
{
	if (m_pCurSample)
	{
		delete m_pCurSample;
		m_pCurSample = nullptr;
	}

	if (m_pBeforeSample)
	{
		delete m_pBeforeSample;
		m_pBeforeSample = nullptr;
	}

}


/**
 * @brief 设置整型参数，用于切换渲染样例。
 *
 * @param paramType 参数类型，目前支持 SAMPLE_TYPE 用于样例切换。
 * @param value0 样例类型的值，对应不同的样例。
 * @param value1 保留参数，暂未使用。
 */
void MyGLRenderContext::SetParamsInt(int paramType, int value0, int value1)
{
    LOGCATE("MyGLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType, value0, value1);

    // 检查参数类型是否为 SAMPLE_TYPE
    if (paramType == SAMPLE_TYPE)
    {
        // 将当前样例保存为之前的样例
        m_pBeforeSample = m_pCurSample;

        LOGCATE("MyGLRenderContext::SetParamsInt 0 m_pBeforeSample = %p", m_pBeforeSample);

        // 根据 value0 值切换到不同的样例对象
        switch (value0)
        {
            case SAMPLE_TYPE_KEY_TRIANGLE:
                // 渲染简单的三角形
                m_pCurSample = new TriangleSample();
                break;
            case SAMPLE_TYPE_KEY_TEXTURE_MAP:
                // 渲染纹理映射示例
                m_pCurSample = new TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_YUV_TEXTURE_MAP:
                // YUV 格式纹理映射
                m_pCurSample = new NV21TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_VAO:
                // VAO（顶点数组对象）示例
                m_pCurSample = new VaoSample();
                break;
            case SAMPLE_TYPE_KEY_FBO:
                // FBO（帧缓冲对象）示例
                m_pCurSample = new FBOSample();
                break;
            case SAMPLE_TYPE_KEY_FBO_LEG_LENGTHEN:
                // 使用 FBO 实现腿部拉伸特效
                m_pCurSample = new FBOLegLengthenSample();
                break;
            case SAMPLE_TYPE_KEY_COORD_SYSTEM:
                // 坐标系变换示例
                m_pCurSample = new CoordSystemSample();
                break;
            case SAMPLE_TYPE_KEY_BASIC_LIGHTING:
                // 基础光照示例
                m_pCurSample = new BasicLightingSample();
                break;
            case SAMPLE_TYPE_KEY_TRANSFORM_FEEDBACK:
                // Transform Feedback 示例
                m_pCurSample = new TransformFeedbackSample();
                break;
            case SAMPLE_TYPE_KEY_MULTI_LIGHTS:
                // 多光源渲染示例
                m_pCurSample = new MultiLightsSample();
                break;
            case SAMPLE_TYPE_KEY_DEPTH_TESTING:
                // 深度测试示例
                m_pCurSample = new DepthTestingSample();
                break;
            case SAMPLE_TYPE_KEY_INSTANCING:
                // 实例化渲染 3D 示例
                m_pCurSample = new Instancing3DSample();
                break;
            case SAMPLE_TYPE_KEY_STENCIL_TESTING:
                // 模板测试示例
                m_pCurSample = new StencilTestingSample();
                break;
            case SAMPLE_TYPE_KEY_BLENDING:
                // 混合效果示例
                m_pCurSample = new BlendingSample();
                break;
            case SAMPLE_TYPE_KEY_PARTICLES:
                // 粒子系统渲染示例
                m_pCurSample = new ParticlesSample();
                break;
            case SAMPLE_TYPE_KEY_SKYBOX:
                // 天空盒示例
                m_pCurSample = new SkyBoxSample();
                break;
            case SAMPLE_TYPE_KEY_3D_MODEL:
                // 加载和渲染 3D 模型示例
                m_pCurSample = new Model3DSample();
                break;
            case SAMPLE_TYPE_KEY_PBO:
                // PBO（像素缓冲对象）示例
                m_pCurSample = new PBOSample();
                break;
            case SAMPLE_TYPE_KEY_BEATING_HEART:
                // 跳动的心脏效果示例
                m_pCurSample = new BeatingHeartSample();
                break;
            case SAMPLE_TYPE_KEY_CLOUD:
                // 云特效示例
                m_pCurSample = new CloudSample();
                break;
            case SAMPLE_TYPE_KEY_TIME_TUNNEL:
                // 时间隧道特效示例
                m_pCurSample = new TimeTunnelSample();
                break;
            case SAMPLE_TYPE_KEY_BEZIER_CURVE:
                // 贝塞尔曲线示例
                m_pCurSample = new BezierCurveSample();
                break;
            case SAMPLE_TYPE_KEY_BIG_EYES:
                // 大眼特效示例
                m_pCurSample = new BigEyesSample();
                break;
            case SAMPLE_TYPE_KEY_FACE_SLENDER:
                // 瘦脸特效示例
                m_pCurSample = new FaceSlenderSample();
                break;
            case SAMPLE_TYPE_KEY_BIG_HEAD:
                // 大头特效示例
                m_pCurSample = new BigHeadSample();
                break;
            case SAMPLE_TYPE_KEY_RATARY_HEAD:
                // 旋转头部特效示例
                m_pCurSample = new RotaryHeadSample();
                break;
            case SAMPLE_TYPE_KEY_VISUALIZE_AUDIO:
                // 音频可视化示例
                m_pCurSample = new VisualizeAudioSample();
                break;
            case SAMPLE_TYPE_KEY_SCRATCH_CARD:
                // 刮刮卡效果示例
                m_pCurSample = new ScratchCardSample();
                break;
            case SAMPLE_TYPE_KEY_AVATAR:
                // 虚拟头像示例
                m_pCurSample = new AvatarSample();
                break;
            case SAMPLE_TYPE_KEY_SHOCK_WAVE:
                // 冲击波特效示例
                m_pCurSample = new ShockWaveSample();
                break;
            case SAMPLE_TYPE_KEY_MRT:
                // MRT（多渲染目标）示例
                m_pCurSample = new MRTSample();
                break;
            case SAMPLE_TYPE_KEY_FBO_BLIT:
                // 使用 FBO 的 Blit 示例
                m_pCurSample = new FBOBlitSample();
                break;
            case SAMPLE_TYPE_KEY_TBO:
                // TBO（纹理缓冲对象）示例
                m_pCurSample = new TextureBufferSample();
                break;
            case SAMPLE_TYPE_KEY_UBO:
                // UBO（统一缓冲对象）示例
                m_pCurSample = new UniformBufferSample();
                break;
            case SAMPLE_TYPE_KEY_RGB2YUYV:
                // RGB 转 YUYV 示例
                m_pCurSample = new RGB2YUYVSample();
                break;
            case SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER:
                // 多线程渲染示例
                m_pCurSample = new SharedEGLContextSample();
                break;
            case SAMPLE_TYPE_KEY_TEXT_RENDER:
                // 文本渲染示例
                m_pCurSample = new TextRenderSample();
                break;
            case SAMPLE_TYPE_KEY_STAY_COLOR:
                // 停留颜色示例
                m_pCurSample = new PortraitStayColorExample();
                break;
            case SAMPLE_TYPE_KEY_TRANSITIONS_1:
                // 转场特效示例 1
                m_pCurSample = new GLTransitionExample();
                break;
            case SAMPLE_TYPE_KEY_TRANSITIONS_2:
                // 转场特效示例 2
                m_pCurSample = new GLTransitionExample_2();
                break;
            case SAMPLE_TYPE_KEY_TRANSITIONS_3:
                // 转场特效示例 3
                m_pCurSample = new GLTransitionExample_3();
                break;
            case SAMPLE_TYPE_KEY_TRANSITIONS_4:
                // 转场特效示例 4
                m_pCurSample = new GLTransitionExample_4();
                break;
            case SAMPLE_TYPE_KEY_RGB2NV21:
                // RGB 转 NV21 示例
                m_pCurSample = new RGB2NV21Sample();
                break;
            case SAMPLE_TYPE_KEY_RGB2I420:
                // RGB 转 I420 示例
                m_pCurSample = new RGB2I420Sample();
                break;
            case SAMPLE_TYPE_KEY_RGB2I444:
                // RGB 转 I444 示例
                m_pCurSample = new RGB2I444Sample();
                break;
            case SAMPLE_TYPE_KEY_COPY_TEXTURE:
                // 拷贝纹理示例
                m_pCurSample = new CopyTextureExample();
                break;
            case SAMPLE_TYPE_KEY_BLIT_FRAME_BUFFER:
                // Blit Frame Buffer 示例
                m_pCurSample = new BlitFrameBufferExample();
                break;
            case SAMPLE_TYPE_KEY_BINARY_PROGRAM:
                // 二进制程序加载示例
                m_pCurSample = new BinaryProgramExample();
                break;
            default:
                // 未知样例，设置为 nullptr
                m_pCurSample = nullptr;
                break;
        }

        LOGCATE("MyGLRenderContext::SetParamsInt m_pBeforeSample = %p, m_pCurSample=%p", m_pBeforeSample, m_pCurSample);
    }
}

void MyGLRenderContext::SetParamsFloat(int paramType, float value0, float value1) {
	LOGCATE("MyGLRenderContext::SetParamsFloat paramType=%d, value0=%f, value1=%f", paramType, value0, value1);
	if(m_pCurSample)
	{
		switch (paramType)
		{
			case SAMPLE_TYPE_KEY_SET_TOUCH_LOC:
				m_pCurSample->SetTouchLocation(value0, value1);
				break;
			case SAMPLE_TYPE_SET_GRAVITY_XY:
                m_pCurSample->SetGravityXY(value0, value1);
				break;
			default:
				break;

		}
	}

}

void MyGLRenderContext::SetParamsShortArr(short *const pShortArr, int arrSize) {
	LOGCATE("MyGLRenderContext::SetParamsShortArr pShortArr=%p, arrSize=%d, pShortArr[0]=%d", pShortArr, arrSize, pShortArr[0]);
	if(m_pCurSample)
	{
		m_pCurSample->LoadShortArrData(pShortArr, arrSize);
	}

}

void MyGLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	LOGCATE("MyGLRenderContext::UpdateTransformMatrix [rotateX, rotateY, scaleX, scaleY] = [%f, %f, %f, %f]", rotateX, rotateY, scaleX, scaleY);
	if (m_pCurSample)
	{
		m_pCurSample->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
	}
}

void MyGLRenderContext::SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData)
{
	LOGCATE("MyGLRenderContext::SetImageDataWithIndex index=%d, format=%d, width=%d, height=%d, pData=%p", index, format, width, height, pData);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pData;

	switch (format)
	{
		case IMAGE_FORMAT_NV12:
		case IMAGE_FORMAT_NV21:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			break;
		case IMAGE_FORMAT_I420:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
			break;
		default:
			break;
	}

	if (m_pCurSample)
	{
		m_pCurSample->LoadMultiImageWithIndex(index, &nativeImage);
	}

}

void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData)
{
	LOGCATE("MyGLRenderContext::SetImageData format=%d, width=%d, height=%d, pData=%p", format, width, height, pData);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pData;

	switch (format)
	{
		case IMAGE_FORMAT_NV12:
		case IMAGE_FORMAT_NV21:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			break;
		case IMAGE_FORMAT_I420:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
			break;
		default:
			break;
	}

	if (m_pCurSample)
	{
		m_pCurSample->LoadImage(&nativeImage);
	}

}

void MyGLRenderContext::OnSurfaceCreated()
{
	LOGCATE("MyGLRenderContext::OnSurfaceCreated");
	glClearColor(1.0f,1.0f,1.0f, 1.0f);
}

void MyGLRenderContext::OnSurfaceChanged(int width, int height)
{
	LOGCATE("MyGLRenderContext::OnSurfaceChanged [w, h] = [%d, %d]", width, height);
	glViewport(0, 0, width, height);
	m_ScreenW = width;
	m_ScreenH = height;
}

void MyGLRenderContext::OnDrawFrame()
{
	LOGCATE("MyGLRenderContext::OnDrawFrame");
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (m_pBeforeSample)
	{
		m_pBeforeSample->Destroy();
		delete m_pBeforeSample;
		m_pBeforeSample = nullptr;
	}

	if (m_pCurSample)
	{
		m_pCurSample->Init();
		m_pCurSample->Draw(m_ScreenW, m_ScreenH);
	}
}

MyGLRenderContext *MyGLRenderContext::GetInstance()
{
	LOGCATE("MyGLRenderContext::GetInstance");
	if (m_pContext == nullptr)
	{
		m_pContext = new MyGLRenderContext();
	}
	return m_pContext;
}

void MyGLRenderContext::DestroyInstance()
{
	LOGCATE("MyGLRenderContext::DestroyInstance");
	if (m_pContext)
	{
		delete m_pContext;
		m_pContext = nullptr;
	}

}