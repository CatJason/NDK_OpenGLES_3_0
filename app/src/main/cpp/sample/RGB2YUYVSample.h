#ifndef NDK_OPENGLES_3_0_RGB2YUYVSAMPLE_H
#define NDK_OPENGLES_3_0_RGB2YUYVSAMPLE_H


#include "GLSampleBase.h"
#include "../util/ImageDef.h"

class RGB2YUYVSample : public GLSampleBase
{
public:
	RGB2YUYVSample();

	virtual ~RGB2YUYVSample();

	virtual void LoadImage(NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int screenW, int screenH);

	virtual void Destroy();

	bool CreateFrameBufferObj();

private:
	GLuint m_ImageTextureId;
	GLuint m_FboTextureId;
	GLuint m_FboId;
	GLuint m_VaoIds[2] = {GL_NONE};
	GLuint m_VboIds[4] = {GL_NONE};
	GLint m_SamplerLoc;
	NativeImage m_RenderImage;
	GLuint m_FboProgramObj;
	GLuint m_FboVertexShader;
	GLuint m_FboFragmentShader;
	GLint m_FboSamplerLoc;
	//GLuint m_YuyvWidth;

};


#endif //NDK_OPENGLES_3_0_RGB2YUYVSAMPLE_H
