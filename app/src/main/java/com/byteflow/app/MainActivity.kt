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

    private fun showGLSampleDialog() {
        val builder = AlertDialog.Builder(this)
        val inflater = LayoutInflater.from(this)
        val rootView = inflater.inflate(R.layout.sample_selected_layout, null)
        val dialog = builder.create()
        val confirmBtn = rootView.findViewById<Button>(R.id.confirm_btn)
        confirmBtn.setOnClickListener { dialog.cancel() }
        val resolutionsListView = rootView.findViewById<RecyclerView>(R.id.resolution_list_view)
        val myPreviewSizeViewAdapter = MyRecyclerViewAdapter(this, Arrays.asList(*SAMPLE_TITLES))
        myPreviewSizeViewAdapter.selectIndex = mSampleSelectedIndex
        myPreviewSizeViewAdapter.addOnItemClickListener(
            object : MyRecyclerViewAdapter.OnItemClickListener{
                override fun onItemClick(view: View?, position: Int) {
                    mRootView?.removeView(mGLSurfaceView)
                    val lp = RelativeLayout.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT
                    )
                    lp.addRule(RelativeLayout.CENTER_IN_PARENT)
                    val glSurfaceView = MyGLSurfaceView(this@MainActivity, mGLRender)
                    mGLSurfaceView = glSurfaceView
                    mRootView?.addView(glSurfaceView, lp)
                    val selectIndex = myPreviewSizeViewAdapter.selectIndex
                    myPreviewSizeViewAdapter.selectIndex = position
                    myPreviewSizeViewAdapter.notifyItemChanged(selectIndex)
                    myPreviewSizeViewAdapter.notifyItemChanged(position)
                    mSampleSelectedIndex = position
                    mGLSurfaceView?.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY
                    if (mRootView!!.width != glSurfaceView.width
                        || mRootView!!.height != glSurfaceView.height
                    ) {
                        glSurfaceView.setAspectRatio(mRootView!!.width, mRootView!!.height)
                    }
                    mGLRender.setParamsInt(
                        MyNativeRender.SAMPLE_TYPE,
                        position + MyNativeRender.SAMPLE_TYPE,
                        0
                    )
                    val sampleType = position + MyNativeRender.SAMPLE_TYPE
                    val tmp: Bitmap?
                    when (sampleType) {
                        MyNativeRender.SAMPLE_TYPE_TRIANGLE -> {}
                        MyNativeRender.SAMPLE_TYPE_TEXTURE_MAP -> loadRGBAImage(R.drawable.dzzz)
                        MyNativeRender.SAMPLE_TYPE_YUV_TEXTURE_MAP -> loadNV21Image()
                        MyNativeRender.SAMPLE_TYPE_VAO -> {}
                        MyNativeRender.SAMPLE_TYPE_FBO, MyNativeRender.SAMPLE_TYPE_KEY_COPY_TEXTURE, MyNativeRender.SAMPLE_TYPE_KEY_BLIT_FRAME_BUFFER -> {
                            val bitmap = loadRGBAImage(R.drawable.lye)
                            mGLSurfaceView!!.setAspectRatio(bitmap!!.width, bitmap.height)
                        }
                        MyNativeRender.SAMPLE_TYPE_FBO_LEG -> loadRGBAImage(R.drawable.leg)
                        MyNativeRender.SAMPLE_TYPE_EGL -> startActivity(
                            Intent(
                                this@MainActivity,
                                EGLActivity::class.java
                            )
                        )
                        MyNativeRender.SAMPLE_TYPE_COORD_SYSTEM,
                        MyNativeRender.SAMPLE_TYPE_BASIC_LIGHTING, MyNativeRender.SAMPLE_TYPE_TRANS_FEEDBACK,
                        MyNativeRender.SAMPLE_TYPE_MULTI_LIGHTS,
                        MyNativeRender.SAMPLE_TYPE_DEPTH_TESTING,
                        MyNativeRender.SAMPLE_TYPE_INSTANCING,
                        MyNativeRender.SAMPLE_TYPE_STENCIL_TESTING -> loadRGBAImage(
                            R.drawable.card
                        )
                        MyNativeRender.SAMPLE_TYPE_BLENDING -> {
                            loadRGBAImage(R.drawable.board_texture, 0)
                            loadRGBAImage(R.drawable.floor, 1)
                            loadRGBAImage(R.drawable.window, 2)
                        }
                        MyNativeRender.SAMPLE_TYPE_PARTICLES -> {
                            loadRGBAImage(R.drawable.board_texture)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_SKYBOX -> {
                            loadRGBAImage(R.drawable.right, 0)
                            loadRGBAImage(R.drawable.left, 1)
                            loadRGBAImage(R.drawable.top, 2)
                            loadRGBAImage(R.drawable.bottom, 3)
                            loadRGBAImage(R.drawable.back, 4)
                            loadRGBAImage(R.drawable.front, 5)
                        }
                        MyNativeRender.SAMPLE_TYPE_PBO -> {
                            loadRGBAImage(R.drawable.front)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_BEATING_HEART -> mGLSurfaceView!!.renderMode =
                            GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        MyNativeRender.SAMPLE_TYPE_KEY_CLOUD -> {
                            loadRGBAImage(R.drawable.noise)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_TIME_TUNNEL -> {
                            loadRGBAImage(R.drawable.front)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_BEZIER_CURVE ->
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        MyNativeRender.SAMPLE_TYPE_KEY_BIG_EYES,
                        MyNativeRender.SAMPLE_TYPE_KEY_FACE_SLENDER -> {
                            val bitmap = loadRGBAImage(R.drawable.yifei)
                            glSurfaceView.setAspectRatio(bitmap!!.width, bitmap.height)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_BIG_HEAD,
                        MyNativeRender.SAMPLE_TYPE_KEY_ROTARY_HEAD -> {
                            val b = loadRGBAImage(R.drawable.huge)
                            glSurfaceView.setAspectRatio(b!!.width, b.height)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_VISUALIZE_AUDIO -> {
                            if (mAudioCollector == null) {
                                mAudioCollector = AudioCollector()
                                mAudioCollector?.addCallback(this@MainActivity)
                                mAudioCollector?.init(this@MainActivity)
                            }
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                            val b1 = loadRGBAImage(R.drawable.yifei)
                            glSurfaceView.setAspectRatio(b1!!.width, b1.height)
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_SCRATCH_CARD -> {
                            val b1 = loadRGBAImage(R.drawable.yifei)
                            glSurfaceView.setAspectRatio(b1!!.width, b1.height)
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_AVATAR -> {
                            val b2 = loadRGBAImage(R.drawable.avatar_a, 0)
                            glSurfaceView.setAspectRatio(b2!!.width, b2.height)
                            loadRGBAImage(R.drawable.avatar_b, 1)
                            loadRGBAImage(R.drawable.avatar_c, 2)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_SHOCK_WAVE,
                        MyNativeRender.SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER,
                        MyNativeRender.SAMPLE_TYPE_KEY_TEXT_RENDER -> {
                            val b3 = loadRGBAImage(R.drawable.lye)
                            glSurfaceView.setAspectRatio(b3!!.width, b3.height)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_MRT,
                        MyNativeRender.SAMPLE_TYPE_KEY_FBO_BLIT,
                        MyNativeRender.SAMPLE_TYPE_KEY_TBO,
                        MyNativeRender.SAMPLE_TYPE_KEY_UBO,
                        MyNativeRender.SAMPLE_TYPE_KEY_BINARY_PROGRAM -> {
                            val b4 = loadRGBAImage(R.drawable.lye)
                            glSurfaceView.setAspectRatio(b4!!.width, b4.height)
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_RGB2YUYV,
                        MyNativeRender.SAMPLE_TYPE_KEY_RGB2NV21,
                        MyNativeRender.SAMPLE_TYPE_KEY_RGB2I420,
                        MyNativeRender.SAMPLE_TYPE_KEY_RGB2I444 -> {
                            tmp = loadRGBAImage(R.drawable.sk)
                            glSurfaceView.setAspectRatio(tmp!!.width, tmp.height)
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_STAY_COLOR -> {
                            loadGrayImage()
                            val b5 = loadRGBAImage(R.drawable.lye2)
                            loadRGBAImage(R.drawable.ascii_mapping, 1)
                            glSurfaceView.setAspectRatio(b5!!.width, b5.height)
                            glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
                        }
                        MyNativeRender.SAMPLE_TYPE_KEY_TRANSITIONS_1,
                        MyNativeRender.SAMPLE_TYPE_KEY_TRANSITIONS_2,
                        MyNativeRender.SAMPLE_TYPE_KEY_TRANSITIONS_3,
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
                    glSurfaceView.requestRender()
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
        val manager = LinearLayoutManager(this)
        manager.orientation = LinearLayoutManager.VERTICAL
        resolutionsListView.layoutManager = manager
        resolutionsListView.adapter = myPreviewSizeViewAdapter
        resolutionsListView.scrollToPosition(mSampleSelectedIndex)
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

    override fun onAudioBufferCallback(buffer: ShortArray?) {
        Log.e(TAG, "onAudioBufferCallback() called with: buffer[0] = [" + buffer!![0] + "]")
        mGLRender.setAudioData(buffer)
    }
}