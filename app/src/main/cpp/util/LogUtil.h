/**
 * @file LogUtil.h
 * @brief 用于 Android 平台的日志工具类，提供日志打印、函数执行时间统计和 OpenGL 错误检查等功能。
 *
 * @author
 * Created by 公众号：字节流动 on 2021/3/12.
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow，领取视频教程，拉你进技术交流群。
 *
 * @details 本工具类封装了 Android 日志功能，包括不同级别的日志打印 (ERROR, VERBOSE, DEBUG, INFO)。
 * 提供了便捷的函数执行时间统计宏，以及 OpenGL 错误检查工具。
 *
 */

#ifndef BYTEFLOW_LOGUTIL_H
#define BYTEFLOW_LOGUTIL_H

#include <android/log.h>
#include <sys/time.h>

// 定义日志的 TAG，所有日志会以此 TAG 开头
#define LOG_TAG "ByteFlow"

// 定义不同级别的日志打印宏
#define LOGCATE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)   ///< 打印 ERROR 级别日志
#define LOGCATV(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__) ///< 打印 VERBOSE 级别日志
#define LOGCATD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)   ///< 打印 DEBUG 级别日志
#define LOGCATI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)    ///< 打印 INFO 级别日志

// 函数执行时间统计宏
#define FUN_BEGIN_TIME(FUN) {\
    LOGCATE("%s:%s func start", __FILE__, FUN); \
    long long t0 = GetSysCurrentTime();         ///< 记录函数开始时间

#define FUN_END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOGCATE("%s:%s func cost time %ldms", __FILE__, FUN, (long)(t1 - t0));} ///< 记录函数结束时间，并计算耗时

// 简化版本的函数执行时间统计宏
#define BEGIN_TIME(FUN) {\
    LOGCATE("%s func start", FUN); \
    long long t0 = GetSysCurrentTime();         ///< 记录函数开始时间

#define END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOGCATE("%s func cost time %ldms", FUN, (long)(t1 - t0));} ///< 记录函数结束时间，并计算耗时

/**
 * @brief 获取当前系统时间，以毫秒为单位。
 *
 * @return 当前时间（毫秒）。
 */
static long long GetSysCurrentTime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long)(time.tv_sec)) * 1000 + time.tv_usec / 1000; ///< 秒转为毫秒，并加上微秒的毫秒部分
    return curTime;
}

// OpenGL 错误检查宏，记录当前函数中 OpenGL 错误信息
#define GO_CHECK_GL_ERROR(...) LOGCATE("CHECK_GL_ERROR %s glGetError = %d, line = %d, ",  __FUNCTION__, glGetError(), __LINE__)

// 调试日志宏，记录当前函数名和所在行号
#define DEBUG_LOGCATE(...) LOGCATE("DEBUG_LOGCATE %s line = %d",  __FUNCTION__, __LINE__)

#endif //BYTEFLOW_LOGUTIL_H