#ifndef NDK_OPENGLES_3_0_BLITFRAMEBUFFEREXAMPLE_H
#define NDK_OPENGLES_3_0_BLITFRAMEBUFFEREXAMPLE_H


#include "GLSampleBase.h"
#include "../util/ImageDef.h"

class BlitFrameBufferExample : public GLSampleBase
{
public:
	BlitFrameBufferExample();

	virtual ~BlitFrameBufferExample();

	virtual void LoadImage(NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int screenW, int screenH);

	virtual void Destroy();

	bool CreateFrameBufferObj();

private:
	GLuint m_ImageTextureId;
	GLuint m_FboTextureId;
	GLuint m_FboId;
	GLuint m_VaoIds[2];
	GLuint m_VboIds[4];
	GLint m_SamplerLoc;
	NativeImage m_RenderImage;
	GLuint m_FboProgramObj;
	GLuint m_FboVertexShader;
	GLuint m_FboFragmentShader;
	GLint m_FboSamplerLoc;

};


#endif //NDK_OPENGLES_3_0_BLITFRAMEBUFFEREXAMPLE_H
