## 技术文档：`BasicLightingSample` 类

### 简介
`BasicLightingSample` 是一个 OpenGL 程序示例类，用于演示基础光照效果和纹理映射。它实现了使用 MVP 矩阵（模型-视图-投影矩阵）来控制三维场景的渲染，以及基本的环境光、漫反射光和高光的光照计算。

---

### 功能概述

1. **光照计算**：
    - 环境光 (`ambient`)：模拟光源对场景的整体照明。
    - 漫反射 (`diffuse`)：模拟光线与物体表面法向量的交互。
    - 高光 (`specular`)：模拟镜面反射的高光效果。

2. **纹理映射**：
    - 使用 2D 纹理将图像绘制到三维模型表面。

3. **矩阵操作**：
    - 使用模型、视图和投影矩阵实现几何变换（旋转、缩放、平移）和视角控制。

4. **顶点数组对象 (VAO) 和顶点缓冲对象 (VBO)**：
    - 使用 VAO 和 VBO 管理顶点数据和绘制逻辑。

---

### 成员变量

| 成员变量          | 描述                                                                                  |
|-------------------|---------------------------------------------------------------------------------------|
| `m_SamplerLoc`    | 用于纹理采样器的着色器变量位置。                                                        |
| `m_MVPMatLoc`     | MVP 矩阵的着色器变量位置。                                                             |
| `m_ModelMatrixLoc`| 模型矩阵的着色器变量位置。                                                             |
| `m_LightPosLoc`   | 光源位置的着色器变量位置。                                                             |
| `m_LightColorLoc` | 光源颜色的着色器变量位置。                                                             |
| `m_ViewPosLoc`    | 视点位置的着色器变量位置。                                                             |
| `m_TextureId`     | OpenGL 中的纹理 ID。                                                                  |
| `m_VaoId`         | 顶点数组对象 ID，用于存储顶点属性配置。                                                |
| `m_VboIds`        | 顶点缓冲对象 ID，用于存储顶点数据。                                                    |
| `m_ModelMatrix`   | 当前模型矩阵，用于几何变换。                                                           |
| `m_AngleX`        | 绕 X 轴的旋转角度。                                                                    |
| `m_AngleY`        | 绕 Y 轴的旋转角度。                                                                    |
| `m_ScaleX`        | 模型的 X 轴缩放因子。                                                                  |
| `m_ScaleY`        | 模型的 Y 轴缩放因子。                                                                  |

---

### 核心方法

#### 1. `Init()`
初始化 OpenGL 程序，包括：
- 创建并绑定纹理。
- 编译顶点和片段着色器。
- 设置 VAO 和 VBO。
- 为顶点属性（位置、纹理坐标、法向量）绑定缓冲数据。

#### 2. `LoadImage(NativeImage *pImage)`
加载外部图像数据，设置为渲染纹理。
- 参数：`pImage` 为包含图像数据的结构体指针。
- 将图像数据拷贝到类的内部成员变量 `m_RenderImage`。

#### 3. `Draw(int screenW, int screenH)`
渲染逻辑，包含以下步骤：
- 设置深度测试以保证正确的渲染顺序。
- 更新 MVP 矩阵以适应当前屏幕宽高比。
- 绑定并上传纹理数据。
- 使用着色器程序绘制包含光照效果的 3D 模型。

#### 4. `UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)`
更新 MVP 矩阵，用于描述模型在三维空间中的位置和相机视角。
- 参数：
    - `angleX`：绕 X 轴旋转角度。
    - `angleY`：绕 Y 轴旋转角度。
    - `ratio`：宽高比。

#### 5. `UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)`
更新模型的旋转和缩放参数。

#### 6. `Destroy()`
销毁 OpenGL 资源，包括着色器程序、纹理、VBO 和 VAO。

---

### 着色器代码解析

#### 顶点着色器
**功能**：
- 计算模型在世界坐标系中的位置。
- 实现光照计算（环境光、漫反射、高光）。
- 输出纹理坐标和光照颜色。

关键代码：
```glsl
vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));
vec3 lightDir = normalize(lightPos - fragPos);
float diff = max(dot(unitNormal, lightDir), 0.0);
diffuse = diffuseStrength * diff * lightColor;
```

#### 片段着色器
**功能**：
- 根据纹理采样颜色和光照计算结果生成最终像素颜色。

关键代码：
```glsl
vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);
outColor = vec4(finalColor, 1.0);
```

---

### 使用说明

1. **初始化与加载纹理**
   调用 `Init()` 方法初始化 OpenGL 程序，随后通过 `LoadImage()` 加载纹理。

2. **设置变换**
   通过 `UpdateTransformMatrix()` 设置模型的旋转和缩放。

3. **绘制场景**
   调用 `Draw()` 方法，传入屏幕宽度和高度，渲染 3D 模型。

4. **销毁资源**
   使用 `Destroy()` 方法释放 OpenGL 资源。

---

### 应用场景
- 教学示例：用于学习基础光照模型和 OpenGL 编程。
- 渲染演示：展示简单的 3D 模型和光照效果。

---

### 注意事项
1. 纹理数据格式需要与 `GL_RGBA` 对应。
2. 确保 OpenGL 环境支持着色器版本 `#version 300 es`。
3. 调用 `Destroy()` 释放所有资源以避免内存泄漏。