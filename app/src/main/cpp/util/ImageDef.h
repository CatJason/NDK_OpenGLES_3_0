
#ifndef NDK_OPENGLES_3_0_IMAGEDEF_H
#define NDK_OPENGLES_3_0_IMAGEDEF_H

#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "sys/stat.h"
#include "stdint.h"
#include "LogUtil.h"

// 定义各种图像格式
#define IMAGE_FORMAT_RGBA           0x01
#define IMAGE_FORMAT_NV21           0x02
#define IMAGE_FORMAT_NV12           0x03
#define IMAGE_FORMAT_I420           0x04
#define IMAGE_FORMAT_YUYV           0x05
#define IMAGE_FORMAT_GRAY           0x06
#define IMAGE_FORMAT_I444           0x07
#define IMAGE_FORMAT_P010           0x08

// 定义各种图像格式扩展名
#define IMAGE_FORMAT_RGBA_EXT       "RGB32"
#define IMAGE_FORMAT_NV21_EXT       "NV21"
#define IMAGE_FORMAT_NV12_EXT       "NV12"
#define IMAGE_FORMAT_I420_EXT       "I420"
#define IMAGE_FORMAT_YUYV_EXT       "YUYV"
#define IMAGE_FORMAT_GRAY_EXT       "GRAY"
#define IMAGE_FORMAT_I444_EXT       "I444"
#define IMAGE_FORMAT_P010_EXT       "P010" //16bit NV21

// 定义矩形结构体
typedef struct NativeRectF
{
    float left;
    float top;
    float right;
    float bottom;
    NativeRectF()
    {
        left = top = right = bottom = 0.0f;
    }
} RectF;

// 定义尺寸结构体
struct SizeF {
    float width;
    float height;
    SizeF() {
        width = height = 0;
    }
};

// 定义图像数据结构体
struct NativeImage
{
    int width;               // 图像宽度
    int height;              // 图像高度
    int format;              // 图像格式
    uint8_t *ppPlane[3];     // 图像平面数据指针

    NativeImage()
    {
        width = 0;
        height = 0;
        format = 0;
        ppPlane[0] = nullptr;
        ppPlane[1] = nullptr;
        ppPlane[2] = nullptr;
    }
};

class NativeImageUtil
{
public:
/**
 * @brief 为指定的 NativeImage 分配内存。
 *
 * 根据图像的格式和尺寸分配内存，并初始化相应的内存平面指针。
 *
 * @param pImage 指向 NativeImage 结构体的指针。
 *               该结构体必须包含正确的宽度、高度和格式信息。
 */
    static void AllocNativeImage(NativeImage *pImage)
    {
        // 如果图像宽度或高度为 0，则不进行任何操作
        if (pImage->height == 0 || pImage->width == 0) return;

        // 根据图像格式分配内存
        switch (pImage->format)
        {
            case IMAGE_FORMAT_RGBA:
            {
                // 为 RGBA 格式分配 (宽度 × 高度 × 4 字节) 的内存
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 4));
            }
                break;

            case IMAGE_FORMAT_YUYV:
            {
                // 为 YUYV 格式分配 (宽度 × 高度 × 2 字节) 的内存
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 2));
            }
                break;

            case IMAGE_FORMAT_NV12:
            case IMAGE_FORMAT_NV21:
            {
                // 为 NV12/NV21 格式分配 (宽度 × 高度 × 1.5 字节) 的内存
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
                // 第二个平面指针指向 Y 平面的后续位置
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
            }
                break;

            case IMAGE_FORMAT_I420:
            {
                // 为 I420 格式分配 (宽度 × 高度 × 1.5 字节) 的内存
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
                // 第二个平面为 U 平面，第三个平面为 V 平面
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
                pImage->ppPlane[2] = pImage->ppPlane[1] + pImage->width * (pImage->height >> 2);
            }
                break;

            case IMAGE_FORMAT_GRAY:
            {
                // 为灰度图像格式分配 (宽度 × 高度) 的内存
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height));
            }
                break;

            case IMAGE_FORMAT_I444:
            {
                // 为 I444 格式分配 (宽度 × 高度 × 3 字节) 的内存
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 3));
            }
                break;

            case IMAGE_FORMAT_P010:
            {
                // 为 P010 格式分配 (宽度 × 高度 × 3 字节) 的内存
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 3));
                // 第二个平面指向第一个平面之后的位置
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height * 2;
            }
                break;

            default:
                // 如果格式不支持，打印日志并跳过
                LOGCATE("NativeImageUtil::AllocNativeImage do not support the format. Format = %d", pImage->format);
                break;
        }
    }

	static void FreeNativeImage(NativeImage *pImage)
	{
		if (pImage == nullptr || pImage->ppPlane[0] == nullptr) return;

		free(pImage->ppPlane[0]);
		pImage->ppPlane[0] = nullptr;
		pImage->ppPlane[1] = nullptr;
		pImage->ppPlane[2] = nullptr;
	}

    /**
     * @brief 拷贝一个 NativeImage 的数据到另一个 NativeImage。
     *
     * 该方法会检查源图像和目标图像的格式、宽度和高度是否一致，只有满足条件时才会进行数据拷贝。
     * 如果目标图像尚未分配内存，会自动分配内存。
     *
     * @param pSrcImg 指向源图像的指针，包含需要拷贝的数据。
     * @param pDstImg 指向目标图像的指针，数据会拷贝到该图像中。
     */
    static void CopyNativeImage(NativeImage *pSrcImg, NativeImage *pDstImg)
    {
        // 检查源图像是否为空或其数据指针是否为空
        if (pSrcImg == nullptr || pSrcImg->ppPlane[0] == nullptr) return;

        // 检查源图像和目标图像的格式、宽度、高度是否一致
        if (pSrcImg->format != pDstImg->format ||
            pSrcImg->width != pDstImg->width ||
            pSrcImg->height != pDstImg->height) return;

        // 如果目标图像尚未分配内存，分配所需内存
        if (pDstImg->ppPlane[0] == nullptr) AllocNativeImage(pDstImg);

        // 根据图像格式拷贝对应的数据
        switch (pSrcImg->format)
        {
            case IMAGE_FORMAT_I420:
            case IMAGE_FORMAT_NV21:
            case IMAGE_FORMAT_NV12:
            {
                // 拷贝 1.5 倍宽度 × 高度的内存数据（包括 Y 平面和 UV 平面）
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 1.5);
            }
                break;

            case IMAGE_FORMAT_YUYV:
            {
                // 拷贝 2 倍宽度 × 高度的内存数据
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 2);
            }
                break;

            case IMAGE_FORMAT_RGBA:
            {
                // 拷贝 4 倍宽度 × 高度的内存数据
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 4);
            }
                break;

            case IMAGE_FORMAT_GRAY:
            {
                // 拷贝宽度 × 高度的内存数据（单通道灰度图像）
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height);
            }
                break;

            case IMAGE_FORMAT_P010:
            case IMAGE_FORMAT_I444:
            {
                // 拷贝 3 倍宽度 × 高度的内存数据
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 3);
            }
                break;

            default:
            {
                // 如果格式不支持，打印日志并跳过操作
                LOGCATE("NativeImageUtil::CopyNativeImage do not support the format. Format = %d", pSrcImg->format);
            }
                break;
        }
    }
/**
 * @brief 将 NativeImage 图像数据保存到文件。
 *
 * 此方法根据图像的格式生成适当的文件扩展名，并将图像数据保存到指定路径和文件中。
 * 如果路径不存在，会自动创建该路径。支持的图像格式包括 I420、NV12、NV21、RGBA、YUYV、GRAY、I444 和 P010。
 *
 * @param pSrcImg 指向需要保存的图像数据的指针。
 * @param pPath 保存文件的路径，不能为空。
 * @param pFileName 保存文件的名称（不含扩展名），不能为空。
 */
    static void DumpNativeImage(NativeImage *pSrcImg, const char *pPath, const char *pFileName)
    {
        // 检查输入参数是否有效
        if (pSrcImg == nullptr || pPath == nullptr || pFileName == nullptr) return;

        // 如果指定路径不存在，创建路径
        if (access(pPath, 0) == -1)
        {
            mkdir(pPath, 0666);
        }

        // 定义图像保存路径和扩展名
        char imgPath[256] = {0};
        const char *pExt = nullptr;

        // 根据图像格式设置文件扩展名
        switch (pSrcImg->format)
        {
            case IMAGE_FORMAT_I420:
                // I420 格式扩展名，例如 .i420
                pExt = IMAGE_FORMAT_I420_EXT;
                break;
            case IMAGE_FORMAT_NV12:
                // NV12 格式扩展名，例如 .nv12
                pExt = IMAGE_FORMAT_NV12_EXT;
                break;
            case IMAGE_FORMAT_NV21:
                // NV21 格式扩展名，例如 .nv21
                pExt = IMAGE_FORMAT_NV21_EXT;
                break;
            case IMAGE_FORMAT_RGBA:
                // RGBA 格式扩展名，例如 .rgba
                pExt = IMAGE_FORMAT_RGBA_EXT;
                break;
            case IMAGE_FORMAT_YUYV:
                // YUYV 格式扩展名，例如 .yuyv
                pExt = IMAGE_FORMAT_YUYV_EXT;
                break;
            case IMAGE_FORMAT_GRAY:
                // 灰度图格式扩展名，例如 .gray
                pExt = IMAGE_FORMAT_GRAY_EXT;
                break;
            case IMAGE_FORMAT_I444:
                // I444 格式扩展名，例如 .i444
                pExt = IMAGE_FORMAT_I444_EXT;
                break;
            case IMAGE_FORMAT_P010:
                // P010 格式扩展名，例如 .p010
                pExt = IMAGE_FORMAT_P010_EXT;
                break;
            default:
                // 未知格式使用默认扩展名
                pExt = "Default";
                break;
        }

        // 构造完整的图像文件路径
        sprintf(imgPath, "%s/IMG_%dx%d_%s.%s", pPath, pSrcImg->width, pSrcImg->height, pFileName, pExt);

        // 打开文件以二进制写入
        FILE *fp = fopen(imgPath, "wb");

        // 记录日志，显示文件指针和文件路径
        LOGCATE("DumpNativeImage fp=%p, file=%s", fp, imgPath);

        // 如果文件打开成功，写入图像数据
        if (fp)
        {
            // 根据图像格式写入数据
            switch (pSrcImg->format)
            {
                case IMAGE_FORMAT_I420:
                {
                    // 写入 Y 平面
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    // 写入 U 平面
                    fwrite(pSrcImg->ppPlane[1], static_cast<size_t>((pSrcImg->width >> 1) * (pSrcImg->height >> 1)), 1, fp);
                    // 写入 V 平面
                    fwrite(pSrcImg->ppPlane[2], static_cast<size_t>((pSrcImg->width >> 1) * (pSrcImg->height >> 1)), 1, fp);
                    break;
                }
                case IMAGE_FORMAT_NV21:
                case IMAGE_FORMAT_NV12:
                {
                    // 写入 Y 平面
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    // 写入 UV 平面
                    fwrite(pSrcImg->ppPlane[1], static_cast<size_t>(pSrcImg->width * (pSrcImg->height >> 1)), 1, fp);
                    break;
                }
                case IMAGE_FORMAT_RGBA:
                {
                    // 写入 RGBA 数据
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height * 4), 1, fp);
                    break;
                }
                case IMAGE_FORMAT_YUYV:
                {
                    // 写入 YUYV 数据
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height * 2), 1, fp);
                    break;
                }
                case IMAGE_FORMAT_GRAY:
                {
                    // 写入灰度图数据
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    break;
                }
                case IMAGE_FORMAT_P010:
                {
                    // 写入 P010 数据
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height * 3), 1, fp);
                    break;
                }
                case IMAGE_FORMAT_I444:
                {
                    // 写入 I444 Y 平面
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    // 写入 I444 U 平面
                    fwrite(pSrcImg->ppPlane[1], static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    // 写入 I444 V 平面
                    fwrite(pSrcImg->ppPlane[2], static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    break;
                }
                default:
                {
                    // 默认写入单平面数据
                    fwrite(pSrcImg->ppPlane[0], static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
                    LOGCATE("DumpNativeImage default");
                    break;
                }
            }

            // 关闭文件
            fclose(fp);
            fp = NULL;
        }
    }
    /**
     * @brief 从指定路径加载图像数据到 NativeImage。
     *
     * 此方法根据图像的格式从文件中读取数据并存储到指定的 NativeImage 结构体中。
     *
     * @param pSrcImg 指向目标 NativeImage 结构体的指针，需确保格式和尺寸已正确设置。
     * @param pPath 指定的文件路径，从中加载图像数据。
     */
    static void LoadNativeImage(NativeImage *pSrcImg, const char *pPath)
    {
        // 检查输入参数是否有效
        if (pSrcImg == nullptr || pPath == nullptr) return;

        // 打开文件，以只读二进制模式读取
        FILE *fp = fopen(pPath, "rb");
        LOGCATE("LoadNativeImage fp=%p, file=%s", fp, pPath);

        // 定义数据大小变量
        int dataSize = 0;

        // 如果文件成功打开
        if (fp)
        {
            // 根据图像格式加载相应的数据
            switch (pSrcImg->format)
            {
                case IMAGE_FORMAT_I420:
                case IMAGE_FORMAT_NV21:
                case IMAGE_FORMAT_NV12:
                {
                    // 计算数据大小：宽度 × 高度 × 1.5（包括 Y 平面和 UV 平面）
                    dataSize = pSrcImg->width * pSrcImg->height * 1.5;
                    // 从文件中读取数据到第一个平面
                    fread(pSrcImg->ppPlane[0], dataSize, 1, fp);
                    break;
                }
                case IMAGE_FORMAT_RGBA:
                {
                    // 计算数据大小：宽度 × 高度 × 4（RGBA 数据）
                    dataSize = pSrcImg->width * pSrcImg->height * 4;
                    // 从文件中读取数据到第一个平面
                    fread(pSrcImg->ppPlane[0], dataSize, 1, fp);
                    break;
                }
                case IMAGE_FORMAT_YUYV:
                {
                    // 计算数据大小：宽度 × 高度 × 2（YUYV 数据）
                    dataSize = pSrcImg->width * pSrcImg->height * 2;
                    // 从文件中读取数据到第一个平面
                    fread(pSrcImg->ppPlane[0], dataSize, 1, fp);
                    break;
                }
                case IMAGE_FORMAT_GRAY:
                {
                    // 计算数据大小：宽度 × 高度（灰度图）
                    dataSize = pSrcImg->width * pSrcImg->height;
                    // 从文件中读取数据到第一个平面
                    fread(pSrcImg->ppPlane[0], dataSize, 1, fp);
                    break;
                }
                case IMAGE_FORMAT_P010:
                case IMAGE_FORMAT_I444:
                {
                    // 计算数据大小：宽度 × 高度 × 3（多平面数据）
                    dataSize = pSrcImg->width * pSrcImg->height * 3;
                    // 从文件中读取数据到第一个平面
                    fread(pSrcImg->ppPlane[0], dataSize, 1, fp);
                    break;
                }
                default:
                {
                    // 不支持的格式，记录日志
                    LOGCATE("LoadNativeImage not support the format %d.", pSrcImg->format);
                    break;
                }
            }

            // 关闭文件
            fclose(fp);
            fp = nullptr;
        }
    }

    /**
     * @brief 将 P010 格式图像转换为 NV21 格式图像。
     *
     * 此方法将 16 位的 P010 格式图像转换为 8 位的 NV21 格式图像。
     * Y 平面和 UV 平面的数据从 P010 中提取并缩减为 8 位。
     *
     * @param pP010Img 指向源图像的指针（P010 格式）。
     * @param pNV21Img 指向目标图像的指针（NV21 格式）。
     * @return 返回值为 0 表示成功，-1 表示失败（如参数无效或格式不匹配）。
     */
    static int ConvertP010toNV21(NativeImage* pP010Img, NativeImage* pNV21Img) {
        // 检查输入参数是否有效
        if (pP010Img == nullptr
            || pNV21Img == nullptr
            || pP010Img->format != IMAGE_FORMAT_P010
            || pNV21Img->format != IMAGE_FORMAT_NV21)
        {
            return -1; // 参数无效或格式不匹配，返回失败
        }

        // 获取图像的宽度和高度
        int width = pP010Img->width, height = pP010Img->height;

        // 转换 Y 平面数据（16 位转 8 位）
        for (int i = 0; i < height; ++i) {
            uint16_t *pu16YData = (uint16_t *)(pP010Img->ppPlane[0] + pP010Img->width * 2 * i); // P010 的 Y 数据指针
            uint8_t  *pu8YData = pNV21Img->ppPlane[0] + pNV21Img->width * i;                   // NV21 的 Y 数据指针
            for (int j = 0; j < width; j++, pu8YData++, pu16YData++) {
                *pu8YData = (uint8_t)(*pu16YData >> 8); // 高 8 位存储到 NV21 的 Y 平面
            }
        }

        // 宽度和高度减半，用于处理 UV 平面
        width /= 2;
        height /= 2;

        // 转换 UV 平面数据（16 位转 8 位）
        for (int i = 0; i < height; ++i) {
            uint16_t *pu16UVData = (uint16_t *)(pP010Img->ppPlane[1] + pP010Img->width * 2 * i); // P010 的 UV 数据指针
            uint8_t  *pu8UVData = pNV21Img->ppPlane[1] + pNV21Img->width * i;                   // NV21 的 UV 数据指针
            for (int j = 0; j < width; ++j, pu8UVData += 2, pu16UVData += 2) {
                *pu8UVData = *pu16UVData >> 8;               // U 数据的高 8 位存储到 NV21 的 UV 平面
                *(pu8UVData + 1) = *(pu16UVData + 1) >> 8;   // V 数据的高 8 位存储到 NV21 的 UV 平面
            }
        }

        return 0; // 转换成功
    }

/**
 * @brief 将 P010 格式的单个平面数据从 16 位转换为 8 位。
 *
 * 此方法将 P010 格式的 16 位数据的高 8 位提取并存储到目标缓冲区中。
 *
 * @param pSrcData 源平面数据的指针，数据为 16 位。
 * @param pDstData 目标平面数据的指针，数据为 8 位。
 * @param width 图像的宽度（以像素为单位）。
 * @param height 图像的高度（以像素为单位）。
 */
    static void ConvertP010PlaneTo8Bit(uint16_t *pSrcData, uint8_t *pDstData, int width, int height) {
        // 检查输入参数是否有效
        if (pSrcData == nullptr || pDstData == nullptr || width <= 0 || height <= 0) return;

        // 遍历图像的每一行
        for (int i = 0; i < height; ++i) {
            // 源平面和目标平面当前行的起始地址
            uint16_t *pu16YData = pSrcData + width * i; // 当前行的 16 位源数据起始地址
            uint8_t  *pu8YData = pDstData + width * i; // 当前行的 8 位目标数据起始地址

            // 遍历当前行的每个像素
            for (int j = 0; j < width; j++, pu8YData++, pu16YData++) {
                // 提取 16 位数据的高 8 位并存储到目标缓冲区
                *pu8YData = (uint8_t)(*pu16YData >> 8);
            }
        }
    }

/**
 * @brief 将 NV21 格式图像转换为 P010 格式图像。
 *
 * 此方法将 NV21 格式的 8 位 Y 和 UV 数据扩展为 P010 格式的 16 位数据。
 * P010 格式中的每个分量（Y、U、V）占用 16 位，但低 8 位填充为 0。
 *
 * @param pNV21Img 指向源图像的指针（NV21 格式）。
 * @param pP010Img 指向目标图像的指针（P010 格式）。
 * @return 返回值为 0 表示成功，-1 表示失败（如参数无效或格式不匹配）。
 */
    static int ConvertNV21toP010(NativeImage* pNV21Img, NativeImage* pP010Img) {
        // 检查输入参数是否有效
        if (pP010Img == nullptr
            || pNV21Img == nullptr
            || pP010Img->format != IMAGE_FORMAT_P010
            || pNV21Img->format != IMAGE_FORMAT_NV21)
        {
            return -1; // 参数无效或格式不匹配，返回失败
        }

        // 获取图像的宽度和高度
        int width = pP010Img->width, height = pP010Img->height;

        // 转换 Y 平面数据（8 位扩展为 16 位）
        for (int i = 0; i < height; ++i) {
            uint16_t *pu16YData = (uint16_t *)(pP010Img->ppPlane[0] + pP010Img->width * 2 * i); // P010 的 Y 数据指针
            uint8_t  *pu8YData = pNV21Img->ppPlane[0] + pNV21Img->width * i;                   // NV21 的 Y 数据指针
            for (int j = 0; j < width; j++, pu8YData++, pu16YData++) {
                *pu16YData = (uint16_t)*pu8YData << 8; // 将 8 位数据左移 8 位，扩展为 16 位
            }
        }

        // 宽度和高度减半，用于处理 UV 平面
        width /= 2;
        height /= 2;

        // 转换 UV 平面数据（8 位扩展为 16 位）
        for (int i = 0; i < height; ++i) {
            uint16_t *pu16UVData = (uint16_t *)(pP010Img->ppPlane[1] + pP010Img->width * 2 * i); // P010 的 UV 数据指针
            uint8_t  *pu8UVData = pNV21Img->ppPlane[1] + pNV21Img->width * i;                   // NV21 的 UV 数据指针
            for (int j = 0; j < width; ++j, pu8UVData += 2, pu16UVData += 2) {
                *pu16UVData = (uint16_t)*pu8UVData << 8;               // U 数据扩展为 16 位
                *(pu16UVData + 1) = (uint16_t)*(pu8UVData + 1) << 8;   // V 数据扩展为 16 位
            }
        }

        return 0; // 转换成功
    }
};


#endif //NDK_OPENGLES_3_0_IMAGEDEF_H
