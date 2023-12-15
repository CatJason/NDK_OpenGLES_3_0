#ifndef NDK_OPENGLES_3_0_COPYTEXTUREEXAMPLE_H
#define NDK_OPENGLES_3_0_COPYTEXTUREEXAMPLE_H


#include "GLSampleBase.h"
#include "../util/ImageDef.h"

class CopyTextureExample : public GLSampleBase
{
public:
	CopyTextureExample();

	virtual ~CopyTextureExample();

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


#endif //NDK_OPENGLES_3_0_COPYTEXTUREEXAMPLE_H
