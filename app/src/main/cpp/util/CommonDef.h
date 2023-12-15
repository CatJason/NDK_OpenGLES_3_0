//
// Created by ByteFlow on 2020/3/10.
//

#ifndef NDK_OPENGLES_3_0_COMMONDEF_H
#define NDK_OPENGLES_3_0_COMMONDEF_H

#include <unistd.h>
#include <cmath>
#include "stdio.h"
#include "stdint.h"

/**
 * 定义一个 2D 点结构体 PointF
 */
typedef struct _tag_PointF
{
    float x; // 点的 X 坐标
    float y; // 点的 Y 坐标

    // 默认构造函数，将坐标初始化为 (0, 0)
    _tag_PointF()
    {
        x = y = 0;
    }

    // 参数化构造函数，初始化为指定的坐标
    _tag_PointF(float xBar, float yBar)
    {
        x = xBar;
        y = yBar;
    }
} PointF;

/**
 * 提供与点相关的常用数学操作工具类
 */
class PointUtil
{
public:
    /**
     * 计算两点之间的欧几里得距离
     * @param p0 第一个点
     * @param p1 第二个点
     * @return 两点之间的距离
     */
    static float Distance(PointF p0, PointF p1)
    {
        return static_cast<float>(sqrt(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2)));
    }

    /**
     * 计算两点之间距离的平方
     * @param p0 第一个点
     * @param p1 第二个点
     * @return 两点之间距离的平方
     */
    static float DistanceSquare(PointF p0, PointF p1)
    {
        return static_cast<float>(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2));
    }

    /**
     * 计算两个点的坐标差
     * @param p0 第一个点
     * @param p1 第二个点
     * @return 返回新的点 (p0 - p1)
     */
    static PointF PointMinus(PointF p0, PointF p1)
    {
        PointF pointF;
        pointF.x = p0.x - p1.x;
        pointF.y = p0.y - p1.y;
        return pointF;
    }

    /**
     * 计算两个点的坐标和
     * @param p0 第一个点
     * @param p1 第二个点
     * @return 返回新的点 (p0 + p1)
     */
    static PointF PointAdd(PointF p0, PointF p1)
    {
        PointF pointF;
        pointF.x = p0.x + p1.x;
        pointF.y = p0.y + p1.y;
        return pointF;
    }

    /**
     * 计算两个点对应坐标相乘
     * @param p0 第一个点
     * @param p1 第二个点
     * @return 返回新的点 (p0.x * p1.x, p0.y * p1.y)
     */
    static PointF PointTimes(PointF p0, PointF p1)
    {
        PointF pointF;
        pointF.x = p0.x * p1.x;
        pointF.y = p0.y * p1.y;
        return pointF;
    }

    /**
     * 计算点与标量相乘
     * @param p0 点
     * @param p1 标量
     * @return 返回新的点 (p0.x * p1, p0.y * p1)
     */
    static PointF PointTimes(PointF p0, float p1)
    {
        PointF pointF;
        pointF.x = p0.x * p1;
        pointF.y = p0.y * p1;
        return pointF;
    }

    /**
     * 计算两个点对应坐标相除
     * @param p0 第一个点
     * @param p1 第二个点
     * @return 返回新的点 (p0.x / p1.x, p0.y / p1.y)
     */
    static PointF PointDivide(PointF p0, PointF p1)
    {
        PointF pointF;
        pointF.x = p0.x / p1.x;
        pointF.y = p0.y / p1.y;
        return pointF;
    }

    /**
     * 计算点与标量相除
     * @param p0 点
     * @param p1 标量
     * @return 返回新的点 (p0.x / p1, p0.y / p1)
     */
    static PointF PointDivide(PointF p0, float p1)
    {
        PointF pointF;
        pointF.x = p0.x / p1;
        pointF.y = p0.y / p1;
        return pointF;
    }
};

#endif //NDK_OPENGLES_3_0_COMMONDEF_H
