package com.byteflow.app

import android.content.Context
import android.util.Log
import java.io.File
import java.io.FileOutputStream

object CommonUtils {
    private const val TAG = "CommonUtils"
    fun copyAssetsDirToSDCard(context: Context, assetsDirName: String, sdCardPath: String) {
        var sdCardPath = sdCardPath
        Log.d(
            TAG,
            "copyAssetsDirToSDCard() called with: context = [$context], assetsDirName = [$assetsDirName], sdCardPath = [$sdCardPath]"
        )
        try {
            val list = context.assets.list(assetsDirName)
            if (list!!.size == 0) {
                val inputStream = context.assets.open(assetsDirName)
                val mByte = ByteArray(1024)
                var bt = 0
                val file = File(
                    sdCardPath + File.separator
                            + assetsDirName.substring(assetsDirName.lastIndexOf('/'))
                )
                if (!file.exists()) {
                    file.createNewFile()
                } else {
                    return
                }
                val fos = FileOutputStream(file)
                while (inputStream.read(mByte).also { bt = it } != -1) {
                    fos.write(mByte, 0, bt)
                }
                fos.flush()
                inputStream.close()
                fos.close()
            } else {
                var subDirName = assetsDirName
                if (assetsDirName.contains("/")) {
                    subDirName = assetsDirName.substring(assetsDirName.lastIndexOf('/') + 1)
                }
                sdCardPath = sdCardPath + File.separator + subDirName
                val file = File(sdCardPath)
                if (!file.exists()) file.mkdirs()
                for (s in list) {
                    copyAssetsDirToSDCard(context, assetsDirName + File.separator + s, sdCardPath)
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}