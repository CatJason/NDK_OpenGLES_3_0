package com.byteflow.app

import android.content.Context
import android.util.Log
import java.io.File
import java.io.FileOutputStream

/**
 * 工具类 `CommonUtils`，提供文件复制等常用方法。
 */
object CommonUtils {
    private const val TAG = "CommonUtils"

    /**
     * 将指定的 Assets 目录及其内容复制到 SD 卡的目标路径下。
     *
     * @param context 上下文对象，用于访问应用的 Assets。
     * @param assetsDirName Assets 目录名称，可以是子目录。
     * @param sdCardPath 目标 SD 卡路径，用于存放复制的文件或目录。
     */
    fun copyAssetsDirToSDCard(context: Context, assetsDirName: String, sdCardPath: String) {
        var sdCardPath = sdCardPath // 更新路径以支持递归
        Log.d(
            TAG,
            "copyAssetsDirToSDCard() called with: context = [$context], assetsDirName = [$assetsDirName], sdCardPath = [$sdCardPath]"
        )
        try {
            // 获取 Assets 目录下的文件或子目录列表
            val list = context.assets.list(assetsDirName)
            if (list!!.isEmpty()) { // 如果列表为空，表示 assetsDirName 是文件
                val inputStream = context.assets.open(assetsDirName) // 打开文件流
                val buffer = ByteArray(1024) // 缓冲区用于文件读取
                var bytesRead: Int
                // 创建目标文件
                val file = File(
                    sdCardPath + File.separator +
                            assetsDirName.substring(assetsDirName.lastIndexOf('/'))
                )
                if (!file.exists()) { // 如果目标文件不存在，则创建
                    file.createNewFile()
                } else {
                    return // 文件已存在，跳过复制
                }
                val fos = FileOutputStream(file) // 输出流，用于写入目标文件
                // 循环读取并写入文件
                while (inputStream.read(buffer).also { bytesRead = it } != -1) {
                    fos.write(buffer, 0, bytesRead)
                }
                fos.flush() // 刷新流
                inputStream.close() // 关闭输入流
                fos.close() // 关闭输出流
            } else { // 如果列表不为空，表示 assetsDirName 是目录
                var subDirName = assetsDirName
                if (assetsDirName.contains("/")) { // 处理多级目录情况
                    subDirName = assetsDirName.substring(assetsDirName.lastIndexOf('/') + 1)
                }
                sdCardPath = sdCardPath + File.separator + subDirName // 更新目标路径
                val file = File(sdCardPath)
                if (!file.exists()) file.mkdirs() // 创建目录
                // 遍历子目录或文件并递归复制
                for (s in list) {
                    copyAssetsDirToSDCard(context, assetsDirName + File.separator + s, sdCardPath)
                }
            }
        } catch (e: Exception) {
            e.printStackTrace() // 打印异常日志
        }
    }
}
