#ifndef NDK_OPENGLES_3_0_CONVEYORBELT_H
#define NDK_OPENGLES_3_0_CONVEYORBELT_H

#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include "GLSampleBase.h"

using namespace glm;

#define BF_LOOP_COUNT 200

#define BF_BANNER_NUM  20
#define BF_TEXTURE_NUM (BF_BANNER_NUM + 1)

class ConveyorBeltExample : public GLSampleBase
{
public:
	ConveyorBeltExample();

	virtual ~ConveyorBeltExample();

	virtual void LoadImage(NativeImage *pImage);

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int screenW, int screenH);

	virtual void Destroy();

	virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

	void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
	GLuint m_TextureIds[BF_TEXTURE_NUM];
	GLint m_MVPMatLoc;
	GLuint m_VaoId;
	GLuint m_VboIds[2];
	NativeImage m_RenderImage;
	NativeImage m_TexImages[BF_TEXTURE_NUM];
    glm::mat4 m_MVPMatrix;

	int m_AngleX;
	int m_AngleY;
	float m_ScaleX;
	float m_ScaleY;

	int m_frameIndex;
	int m_loopCount;

    vec3 m_pVtxCoords[BF_TEXTURE_NUM* 2 * 3];
    vec2 m_pTexCoords[BF_TEXTURE_NUM* 2 * 3];

};


#endif //NDK_OPENGLES_3_0_CONVEYORBELT_H
