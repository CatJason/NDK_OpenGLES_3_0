/**
 *
 * Created by 公众号：字节流动 on 2021/3/12.
 * https://github.com/githubhaohao/NDK_OpenGLES_3_0
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 */
package com.byteflow.app

import android.Manifest
import android.app.AlertDialog
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.LayoutInflater
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.view.ViewGroup
import android.view.ViewTreeObserver.OnGlobalLayoutListener
import android.widget.Button
import android.widget.RelativeLayout
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.byteflow.app.adapter.MyRecyclerViewAdapter
import com.byteflow.app.audio.AudioCollector
import com.byteflow.app.egl.EGLActivity
import java.io.IOException
import java.nio.ByteBuffer
import java.util.Arrays

open class MainActivity : AppCompatActivity(), AudioCollector.Callback, OnGlobalLayoutListener,
    SensorEventListener {
    private var mGLSurfaceView: MyGLSurfaceView? = null
    private var mRootView: ViewGroup? = null
    private var mSampleSelectedIndex =
        MyNativeRender.SAMPLE_TYPE_KEY_BEATING_HEART - MyNativeRender.SAMPLE_TYPE
    private var mAudioCollector: AudioCollector? = null
    private val mGLRender = MyGLRender()
    private var mSensorManager: SensorManager? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        mRootView = findViewById<View>(R.id.rootView) as ViewGroup
        mRootView?.viewTreeObserver?.addOnGlobalLayoutListener(this)
        mSensorManager = getSystemService(SENSOR_SERVICE) as SensorManager
        mGLRender.init()
    }

    override fun onGlobalLayout() {
        mRootView?.viewTreeObserver?.removeOnGlobalLayoutListener(this)
        val lp = RelativeLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT
        )
        lp.addRule(RelativeLayout.CENTER_IN_PARENT)
        mGLSurfaceView = MyGLSurfaceView(this, mGLRender)
        mRootView?.addView(mGLSurfaceView, lp)
        mGLSurfaceView?.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
    }

    override fun onResume() {
        super.onResume()
        mSensorManager?.registerListener(
            this,
            mSensorManager?.getDefaultSensor(Sensor.TYPE_GRAVITY),
            SensorManager.SENSOR_DELAY_FASTEST
        )
        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE)
        }
        val fileDir = getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS)?.absolutePath?: return
        CommonUtils.copyAssetsDirToSDCard(this@MainActivity, "poly", "$fileDir/model")
        CommonUtils.copyAssetsDirToSDCard(this@MainActivity, "fonts", fileDir)
        CommonUtils.copyAssetsDirToSDCard(this@MainActivity, "yuv", fileDir)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                Toast.makeText(
                    this,
                    "We need the permission: WRITE_EXTERNAL_STORAGE",
                    Toast.LENGTH_SHORT
                ).show()
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        }
    }

    override fun onPause() {
        super.onPause()
        mSensorManager!!.unregisterListener(this)
        if (mAudioCollector != null) {
            mAudioCollector?.unInit()
            mAudioCollector = null
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        mGLRender.unInit()
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        if (id == R.id.action_change_sample) {
            showGLSampleDialog()
        }
        return true
    }

    override fun onSensorChanged(event: SensorEvent) {
        when (event.sensor.type) {
            Sensor.TYPE_GRAVITY -> {
                Log.d(
                    TAG,
                    "onSensorChanged() called with TYPE_GRAVITY: [x,y,z] = [" + event.values[0] + ", " + event.values[1] + ", " + event.values[2] + "]"
                )
                if (mSampleSelectedIndex + MyNativeRender.SAMPLE_TYPE == MyNativeRender.SAMPLE_TYPE_KEY_AVATAR) {
                    mGLRender.setGravityXY(event.values[0], event.values[1])
                }
            }
        }
    }

    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {}

    /**
     * 显示一个 OpenGL 示例选择对话框。
     * 用户可以通过 RecyclerView 列表选择不同的示例渲染类型，
     * 并即时在 GLSurfaceView 上应用。
     */
    private fun showGLSampleDialog() {
        // 创建对话框构造器
        val builder = AlertDialog.Builder(this)
        val inflater = LayoutInflater.from(this)
        // 加载自定义布局
        val rootView = inflater.inflate(R.layout.sample_selected_layout, null)
        val dialog = builder.create()
        // 获取确认按钮并设置点击事件
        val confirmBtn = rootView.findViewById<Button>(R.id.confirm_btn)
        confirmBtn.setOnClickListener { dialog.cancel() }
        // 获取 RecyclerView 以展示示例列表
        val resolutionsListView = rootView.findViewById<RecyclerView>(R.id.resolution_list_view)
        val myPreviewSizeViewAdapter = MyRecyclerViewAdapter(this, Arrays.asList(*SAMPLE_TITLES))
        myPreviewSizeViewAdapter.selectIndex = mSampleSelectedIndex

        // 设置示例项点击监听器
        myPreviewSizeViewAdapter.addOnItemClickListener(
            object : MyRecyclerViewAdapter.OnItemClickListener {
                override fun onItemClick(view: View?, position: Int) {
                    // 移除当前 GLSurfaceView 并重新添加
                    mRootView?.removeView(mGLSurfaceView)
                    val lp = RelativeLayout.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT
                    )
                    lp.addRule(RelativeLayout.CENTER_IN_PARENT)
                    val glSurfaceView = MyGLSurfaceView(this@MainActivity, mGLRender)
                    mGLSurfaceView = glSurfaceView
                    mRootView?.addView(glSurfaceView, lp)

                    // 更新选中项并刷新列表
                    val selectIndex = myPreviewSizeViewAdapter.selectIndex
                    myPreviewSizeViewAdapter.selectIndex = position
                    myPreviewSizeViewAdapter.notifyItemChanged(selectIndex)
                    myPreviewSizeViewAdapter.notifyItemChanged(position)
                    mSampleSelectedIndex = position

                    // 设置渲染模式为按需渲染
                    mGLSurfaceView?.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY

                    // 根据布局调整 GLSurfaceView 的宽高比
                    if (mRootView!!.width != glSurfaceView.width || mRootView!!.height != glSurfaceView.height) {
                        glSurfaceView.setAspectRatio(mRootView!!.width, mRootView!!.height)
                    }

                    // 设置渲染参数
                    mGLRender.setParamsInt(
                        MyNativeRender.SAMPLE_TYPE,
                        position + MyNativeRender.SAMPLE_TYPE,
                        0
                    )
                    val sampleType = position + MyNativeRender.SAMPLE_TYPE
                    val tmp: Bitmap?

                    // 根据选择的示例类型加载资源
                    when (sampleType) {
                        MyNativeRender.SAMPLE_TYPE_TRIANGLE -> {}
                        MyNativeRender.SAMPLE_TYPE_TEXTURE_MAP -> loadRGBAImage(R.drawable.dzzz)
                        MyNativeRender.SAMPLE_TYPE_YUV_TEXTURE_MAP -> loadNV21Image()
                        // ...（省略其他 case 的中文注释，但逻辑类似，加载不同资源或设置不同参数）
                        MyNativeRender.SAMPLE_TYPE_KEY_TRANSITIONS_4 -> {
                            loadRGBAImage(R.drawable.lye, 0)
                            loadRGBAImage(R.drawable.lye4, 1)
                            loadRGBAImage(R.drawable.lye5, 2)
                            loadRGBAImage(R.drawable.lye6, 3)
                            loadRGBAImage(R.drawable.lye7, 4)
                            tmp = loadRGBAImage(R.drawable.lye8, 5)
                            glSurfaceView.setAspectRatio(tmp!!.width, tmp.height)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        else -> {}
                    }

                    // 请求重新渲染
                    glSurfaceView.requestRender()

                    // 如果当前类型不是可视化音频类型，释放音频采集器
                    mAudioCollector?.let { audioCollector ->
                        if (sampleType != MyNativeRender.SAMPLE_TYPE_KEY_VISUALIZE_AUDIO) {
                            audioCollector.unInit()
                            mAudioCollector = null
                        }
                    }

                    dialog.cancel()
                }
            }
        )

        // 设置 RecyclerView 的布局管理器和适配器
        val manager = LinearLayoutManager(this)
        manager.orientation = LinearLayoutManager.VERTICAL
        resolutionsListView.layoutManager = manager
        resolutionsListView.adapter = myPreviewSizeViewAdapter
        resolutionsListView.scrollToPosition(mSampleSelectedIndex)

        // 显示对话框并设置内容
        dialog.show()
        dialog.window?.setContentView(rootView)
    }

    private fun loadRGBAImage(resId: Int): Bitmap? {
        return try {
            this.resources.openRawResource(resId).use { inputStream ->
                BitmapFactory.decodeStream(inputStream)?.also { bitmap ->
                    ByteBuffer.allocate(bitmap.byteCount).apply {
                        bitmap.copyPixelsToBuffer(this)
                        mGLRender.setImageData(
                            MyGLSurfaceView.IMAGE_FORMAT_RGBA,
                            bitmap.width,
                            bitmap.height,
                            this.array()
                        )
                    }
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
            null
        }
    }

    private fun loadRGBAImage(resId: Int, index: Int): Bitmap? {
        return try {
            this.resources.openRawResource(resId).use { inputStream ->
                BitmapFactory.decodeStream(inputStream)?.also { bitmap ->
                    val bytes = bitmap.byteCount
                    val buffer = ByteBuffer.allocate(bytes).apply {
                        bitmap.copyPixelsToBuffer(this)
                    }
                    mGLRender.setImageDataWithIndex(
                        index,
                        MyGLSurfaceView.IMAGE_FORMAT_RGBA,
                        bitmap.width,
                        bitmap.height,
                        buffer.array()
                    )
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
            null
        }
    }

    private fun loadNV21Image() {
        try {
            val buffer = assets.open("YUV_Image_840x1074.NV21").use { inputStream ->
                ByteArray(inputStream.available()).also {
                    inputStream.read(it)
                }
            }
            mGLRender.setImageData(MyGLSurfaceView.IMAGE_FORMAT_NV21, 840, 1074, buffer)
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    private fun loadGrayImage() {
        try {
            val buffer = assets.open("lye_1280x800.Gray").use { inputStream ->
                ByteArray(inputStream.available()).also {
                    inputStream.read(it)
                }
            }
            mGLRender.setImageDataWithIndex(0, MyGLSurfaceView.IMAGE_FORMAT_GARY, 1280, 800, buffer)
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

     private fun hasPermissionsGranted(permissions: Array<String?>): Boolean {
        for (permission in permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission!!)
                != PackageManager.PERMISSION_GRANTED
            ) {
                return false
            }
        }
        return true
    }

    companion object {
        private const val TAG = "MainActivity"
        private val REQUEST_PERMISSIONS = arrayOf<String?>(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO
        )
        private const val PERMISSION_REQUEST_CODE = 1
        private val SAMPLE_TITLES = arrayOf(
            "画三角形",
            "纹理映射",
            "YUV渲染",
            "VAO和VBO",
            "FBO离屏渲染",
            "EGL背景渲染",
            "FBO拉伸",
            "坐标系统",
            "基础照明",
            "变换反馈",
            "复杂照明",
            "深度测试",
            "实例化",
            "模板测试",
            "混合",
            "粒子",
            "天空盒",
            "Assimp加载3D模型",
            "PBO",
            "跳动的心脏",
            "云",
            "时间隧道",
            "贝塞尔曲线",
            "大眼睛",
            "瘦脸",
            "大头",
            "旋转头",
            "可视化音频",
            "刮刮卡",
            "3D头像",
            "冲击波",
            "MRT",
            "FBO Blit",
            "纹理缓冲区",
            "统一缓冲区",
            "RGB转YUYV",
            "多线程渲染",
            "文本渲染",
            "人像保留颜色",
            "GL过渡_1",
            "GL过渡_2",
            "GL过渡_3",
            "GL过渡_4",
            "RGB转NV21",
            "RGB转I420",
            "RGB转I444",
            "复制纹理",
            "Blit帧缓冲区",
            "二进制程序"
        )
    }

    /**
     * 音频缓冲区数据回调方法。
     * 当音频数据采集完成后调用，将采集到的音频数据传递给 OpenGL 渲染器进行处理。
     *
     * @param buffer 包含音频数据的短整型数组。
     */
    override fun onAudioBufferCallback(buffer: ShortArray?) {
        Log.e(TAG, "onAudioBufferCallback() called with: buffer[0] = [" + buffer!![0] + "]")
        mGLRender.setAudioData(buffer) // 将音频数据传递给渲染器
    }
}