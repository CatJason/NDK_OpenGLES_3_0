#include "TriangleSample.h"
#include "../util/GLUtils.h"
#include "../util/LogUtil.h"

TriangleSample::TriangleSample() {

}

TriangleSample::~TriangleSample() {
}

void TriangleSample::LoadImage(NativeImage *pImage) {
    //null implement
}

void TriangleSample::Init() {
    if (m_ProgramObj != 0)
        return;
    // 顶点着色器代码
    char vShaderStr[] =
            "#version 300 es                          \n" // 声明使用 OpenGL ES 3.0 版本
            "layout(location = 0) in vec4 vPosition;  \n" // 输入顶点位置，绑定到位置 0
            "layout(location = 1) in vec3 vColor;     \n" // 输入顶点颜色，绑定到位置 1
            "out vec3 fColor;                         \n" // 输出到片段着色器的颜色变量
            "void main()                              \n" // 主函数
            "{                                        \n"
            "   gl_Position = vPosition;              \n" // 将顶点位置传递给 gl_Position（OpenGL 内置变量）
            "   fColor = vColor;                      \n" // 将顶点颜色赋值给输出变量 fColor
            "}                                        \n";

    // 片段着色器代码
    char fShaderStr[] =
            "#version 300 es                              \n" // 声明使用 OpenGL ES 3.0 版本
            "precision mediump float;                     \n" // 指定默认精度为中等精度
            "in vec3 fColor;                              \n" // 从顶点着色器接收的颜色变量
            "out vec4 fragColor;                          \n" // 输出的片段颜色
            "void main()                                  \n" // 主函数
            "{                                            \n"
            "   fragColor = vec4(fColor, 1.0);            \n" // 将 fColor 赋值给 fragColor，同时指定 alpha 为 1.0
            "}                                            \n";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
}

void TriangleSample::Draw(int screenW, int screenH) {
    LOGCATE("TriangleSample::Draw");

    // 定义三角形的顶点数据，包括位置和颜色
    GLfloat vVertices[] = {
            // 顶点位置         // 颜色 (RGB)
            0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 顶部顶点 (红色)
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下角顶点 (绿色)
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // 右下角顶点 (蓝色)
    };

    // 检查是否已经创建了着色器程序对象
    if (m_ProgramObj == 0)
        return;

    // 清除颜色缓冲区、深度缓冲区和模板缓冲区
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置清屏背景颜色为白色
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // 使用着色器程序
    glUseProgram(m_ProgramObj);

    // 加载顶点数据
    // 设置顶点位置数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), vVertices);
    glEnableVertexAttribArray(0);

    // 设置顶点颜色数据
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), vVertices + 3);
    glEnableVertexAttribArray(1);

    // 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 禁用着色器程序
    glUseProgram(GL_NONE);
}

void TriangleSample::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = GL_NONE;
    }
}