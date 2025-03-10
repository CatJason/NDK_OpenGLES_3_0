#include <GLUtils.h>
#include "RGB2I444Sample.h"

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1

RGB2I444Sample::RGB2I444Sample()
{
	m_ImageTextureId = GL_NONE;
	m_FboTextureId = GL_NONE;
	m_SamplerLoc = GL_NONE;
	m_FboId = GL_NONE;
	m_FboProgramObj = GL_NONE;
	m_FboVertexShader = GL_NONE;
	m_FboFragmentShader = GL_NONE;
	m_FboSamplerLoc = GL_NONE;
}

RGB2I444Sample::~RGB2I444Sample()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void RGB2I444Sample::LoadImage(NativeImage *pImage)
{
	LOGCATE("RGB2I444Sample::LoadImage pImage = %p", pImage->ppPlane[0]);
	if (pImage)
	{
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		m_RenderImage.format = pImage->format;
		NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
	}
}

void RGB2I444Sample::Init()
{
	//顶点坐标
	GLfloat vVertices[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
	};

	//正常纹理坐标
	GLfloat vTexCoors[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

	//fbo 纹理坐标与正常纹理方向不同，原点位于左下角
	GLfloat vFboTexCoors[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
	};

	GLushort indices[] = { 0, 1, 2, 1, 3, 2 };

	char vShaderStr[] =
			"#version 300 es                            \n"
			"layout(location = 0) in vec4 a_position;   \n"
			"layout(location = 1) in vec2 a_texCoord;   \n"
			"out vec2 v_texCoord;                       \n"
			"void main()                                \n"
			"{                                          \n"
			"   gl_Position = a_position;               \n"
			"   v_texCoord = a_texCoord;                \n"
			"}                                          \n";

//	// 用于屏幕渲染的片段着色器脚本，渲染 NV21 格式图像
//	char fNV21ShaderStr[] =
//			"#version 300 es\n"
//            "precision mediump float;\n"
//            "in vec2 v_texCoord;\n"
//            "layout(location = 0) out vec4 outColor;\n"
//            "uniform sampler2D s_Texture0;\n"
//            "uniform sampler2D s_Texture1;\n"
//            "\n"
//            "const mat3 YUV2RGB_COEF = mat3( 1.0,     1.0,       1.0,\n"
//            "                                0.0, \t-0.344, \t1.770,\n"
//            "                                1.403,  -0.714,     0.0);\n"
//            "\n"
//            "void main()\n"
//            "{\n"
//            "    vec3 yuv;\n"
//            "    yuv.x = texture2D(s_Texture0, v_texCoord).r;\n"
//            "    yuv.y = texture2D(s_Texture1, v_texCoord).g - 0.5;\n"
//            "    yuv.z = texture2D(s_Texture1, v_texCoord).a - 0.5;\n"
//            "    vec3 rgb = YUV2RGB_COEF * yuv;\n"
//            "    outColor = vec4(rgb, 1.0);\n"
//            "}";

    // 用于普通渲染的片段着色器脚本，简单纹理映射
    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(s_TextureMap, v_texCoord);\n"
            "}";


	// 用于离屏渲染的片段着色器脚本，RGB to YUV
	char fFboShaderStr[] =
			"#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "uniform float u_Offset;\n"
            "//Y =  0.299R + 0.587G + 0.114B\n"
            "//U = -0.147R - 0.289G + 0.436B\n"
            "//V =  0.615R - 0.515G - 0.100B\n"
            "const vec3 COEF_Y = vec3( 0.299,  0.587,  0.114);\n"
            "const vec3 COEF_U = vec3(-0.147, -0.289,  0.436);\n"
            "const vec3 COEF_V = vec3( 0.615, -0.515, -0.100);\n"
            "const float U_DIVIDE_LINE = 1.0 / 3.0;\n"
            "const float V_DIVIDE_LINE = 2.0 / 3.0;\n"
            "void main()\n"
            "{\n"
            "    vec2 texelOffset = vec2(u_Offset, 0.0);\n"
            "    if(v_texCoord.y <= U_DIVIDE_LINE) {\n"
            "        vec2 texCoord = vec2(v_texCoord.x, v_texCoord.y * 3.0);\n"
            "        vec4 color0 = texture(s_TextureMap, texCoord);\n"
            "        vec4 color1 = texture(s_TextureMap, texCoord + texelOffset);\n"
            "        vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 2.0);\n"
            "        vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 3.0);\n"
            "\n"
            "        float y0 = dot(color0.rgb, COEF_Y);\n"
            "        float y1 = dot(color1.rgb, COEF_Y);\n"
            "        float y2 = dot(color2.rgb, COEF_Y);\n"
            "        float y3 = dot(color3.rgb, COEF_Y);\n"
            "        outColor = vec4(y0, y1, y2, y3);\n"
            "    } else if(v_texCoord.y <= V_DIVIDE_LINE) {\n"
            "        vec2 texCoord = vec2(v_texCoord.x, (v_texCoord.y - U_DIVIDE_LINE) * 3.0);\n"
            "        vec4 color0 = texture(s_TextureMap, texCoord);\n"
            "        vec4 color1 = texture(s_TextureMap, texCoord + texelOffset);\n"
            "        vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 2.0);\n"
            "        vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 3.0);\n"
            "\n"
            "        float u0 = dot(color0.rgb, COEF_U) + 0.5;\n"
            "        float u1 = dot(color1.rgb, COEF_U) + 0.5;\n"
            "        float u2 = dot(color2.rgb, COEF_U) + 0.5;\n"
            "        float u3 = dot(color3.rgb, COEF_U) + 0.5;\n"
            "        outColor = vec4(u0, u1, u2, u3);\n"
            "    } else {\n"
            "        vec2 texCoord = vec2(v_texCoord.x, (v_texCoord.y - V_DIVIDE_LINE) * 3.0);\n"
            "        vec4 color0 = texture(s_TextureMap, texCoord);\n"
            "        vec4 color1 = texture(s_TextureMap, texCoord + texelOffset);\n"
            "        vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 2.0);\n"
            "        vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 3.0);\n"
            "\n"
            "        float v0 = dot(color0.rgb, COEF_V) + 0.5;\n"
            "        float v1 = dot(color1.rgb, COEF_V) + 0.5;\n"
            "        float v2 = dot(color2.rgb, COEF_V) + 0.5;\n"
            "        float v3 = dot(color3.rgb, COEF_V) + 0.5;\n"
            "        outColor = vec4(v0, v1, v2, v3);\n"
            "    }\n"
            "}";

	// 编译链接用于普通渲染的着色器程序
	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

	// 编译链接用于离屏渲染的着色器程序
	m_FboProgramObj = GLUtils::CreateProgram(vShaderStr, fFboShaderStr, m_FboVertexShader, m_FboFragmentShader);

	if (m_ProgramObj == GL_NONE || m_FboProgramObj == GL_NONE)
	{
		LOGCATE("RGB2I444Sample::Init m_ProgramObj == GL_NONE");
		return;
	}
	m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
	m_FboSamplerLoc = glGetUniformLocation(m_FboProgramObj, "s_TextureMap");

	// 生成 VBO ，加载顶点数据和索引数据
	// Generate VBO Ids and load the VBOs with data
	glGenBuffers(4, m_VboIds);
	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoors), vTexCoors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vFboTexCoors), vFboTexCoors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GO_CHECK_GL_ERROR();

	// 生成 2 个 VAO，一个用于普通渲染，另一个用于离屏渲染
	// Generate VAO Ids
	glGenVertexArrays(2, m_VaoIds);
    // 初始化用于普通渲染的 VAO
	// Normal rendering VAO
	glBindVertexArray(m_VaoIds[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glEnableVertexAttribArray(VERTEX_POS_INDX);
	glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glEnableVertexAttribArray(TEXTURE_POS_INDX);
	glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
	GO_CHECK_GL_ERROR();
	glBindVertexArray(GL_NONE);


	// 初始化用于离屏渲染的 VAO
	// FBO off screen rendering VAO
	glBindVertexArray(m_VaoIds[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glEnableVertexAttribArray(VERTEX_POS_INDX);
	glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
	glEnableVertexAttribArray(TEXTURE_POS_INDX);
	glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
	GO_CHECK_GL_ERROR();
	glBindVertexArray(GL_NONE);

	// 创建并初始化图像纹理
	glGenTextures(1, &m_ImageTextureId);
	glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	GO_CHECK_GL_ERROR();

	if (!CreateFrameBufferObj())
	{
		LOGCATE("RGB2I444Sample::Init CreateFrameBufferObj fail");
		return;
	}

}

void RGB2I444Sample::Draw(int screenW, int screenH)
{
	// 离屏渲染
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	// Do FBO off screen rendering
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    // 渲染成 I444 ，glviewport 宽度 1/4 * width, 高度 height * 3
    glViewport(0, 0, m_RenderImage.width / 4, m_RenderImage.height * 3);
	glUseProgram(m_FboProgramObj);
	glBindVertexArray(m_VaoIds[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
	glUniform1i(m_FboSamplerLoc, 0);
	float texelOffset = (float) (1.f / (float) m_RenderImage.width);
	GLUtils::setFloat(m_FboProgramObj, "u_Offset", texelOffset);
	GO_CHECK_GL_ERROR();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
	GO_CHECK_GL_ERROR();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//I444 buffer = width * height * 3;
	uint8_t *pBuffer = new uint8_t[m_RenderImage.width * m_RenderImage.height * 3];

	NativeImage nativeImage = m_RenderImage;
	nativeImage.format = IMAGE_FORMAT_I444;
	nativeImage.ppPlane[0] = pBuffer;
	nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + m_RenderImage.width * m_RenderImage.height;
    nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + m_RenderImage.width * m_RenderImage.height;
	FUN_BEGIN_TIME("FBO glReadPixels")
		glReadPixels(0, 0, nativeImage.width / 4, nativeImage.height * 3, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
	FUN_END_TIME("FBO cost glReadPixels")

	std::string path(DEFAULT_OGL_ASSETS_DIR);
	NativeImageUtil::DumpNativeImage(&nativeImage, path.c_str(), "RGB2I444");
	delete []pBuffer;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 普通渲染
	// Do normal rendering
	glViewport(0, 0, screenW, screenH);
	glUseProgram(m_ProgramObj);
	GO_CHECK_GL_ERROR();
	glBindVertexArray(m_VaoIds[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
	glUniform1i(m_SamplerLoc, 0);
	GO_CHECK_GL_ERROR();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
	GO_CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glBindVertexArray(GL_NONE);

}

void RGB2I444Sample::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
	}

	if (m_FboProgramObj)
	{
		glDeleteProgram(m_FboProgramObj);
	}

	if (m_ImageTextureId)
	{
		glDeleteTextures(1, &m_ImageTextureId);
	}

	if (m_FboTextureId)
	{
		glDeleteTextures(1, &m_FboTextureId);
	}

	if (m_VboIds[0])
	{
		glDeleteBuffers(4, m_VboIds);
	}

	if (m_VaoIds[0])
	{
		glDeleteVertexArrays(2, m_VaoIds);
	}

	if (m_FboId)
	{
		glDeleteFramebuffers(1, &m_FboId);
	}

}

bool RGB2I444Sample::CreateFrameBufferObj()
{
	// 创建并初始化 FBO 纹理
	glGenTextures(1, &m_FboTextureId);
	glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	// 创建并初始化 FBO
	glGenFramebuffers(1, &m_FboId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
	glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width / 4, m_RenderImage.height * 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
		LOGCATE("RGB2I444Sample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	return true;

}
