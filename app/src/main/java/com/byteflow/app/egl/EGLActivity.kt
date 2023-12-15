package com.byteflow.app.egl

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.opengl.GLException
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.Button
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.byteflow.app.R
import java.io.IOException
import java.nio.ByteBuffer
import java.nio.IntBuffer

/**
 * EGLActivity 类，用于展示 OpenGL 渲染结果并通过菜单切换不同的着色器效果。
 */
class EGLActivity : AppCompatActivity() {
    private var mImageView: ImageView? = null // 用于显示渲染结果的 ImageView
    private var mBtn: Button? = null // 控制渲染的按钮
    private var mBgRender: NativeEglRender? = null // 本地渲染器对象

    /**
     * 初始化 Activity，设置布局和事件监听器。
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_egl)
        mImageView = findViewById<View>(R.id.imageView) as ImageView
        val button = findViewById<View>(R.id.button) as Button
        mBtn = button
        mBgRender = NativeEglRender()
        mBgRender?.native_EglRenderInit()
        button.setOnClickListener {
            if (button.text == this@EGLActivity.resources.getString(R.string.btn_txt_reset)) {
                mImageView?.setImageResource(R.drawable.leg)
                button.setText(R.string.bg_render_txt)
            } else {
                startBgRender()
                button.setText(R.string.btn_txt_reset)
            }
        }
    }

    /**
     * 在 Activity 销毁时释放本地渲染器资源。
     */
    override fun onDestroy() {
        super.onDestroy()
        mBgRender?.native_EglRenderUnInit()
    }

    /**
     * 创建菜单。
     */
    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_egl, menu) // 加载菜单布局
        return true
    }

    /**
     * 菜单项点击事件处理。
     */
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        var shaderIndex = 0
        when (id) {
            R.id.action_shader0 -> shaderIndex = 0
            R.id.action_shader1 -> shaderIndex = 1
            R.id.action_shader2 -> shaderIndex = 2
            R.id.action_shader3 -> shaderIndex = 3
            R.id.action_shader4 -> shaderIndex = 4
            R.id.action_shader5 -> shaderIndex = 5
            R.id.action_shader6 -> shaderIndex = 6
        }
        mBgRender?.let {
            it.native_EglRenderSetIntParams(PARAM_TYPE_SHADER_INDEX, shaderIndex)
            startBgRender()
            mBtn?.setText(R.string.btn_txt_reset)
        }
        return true
    }

    /**
     * 开始渲染背景图像。
     */
    private fun startBgRender() {
        loadRGBAImage(R.drawable.leg, mBgRender)
        mBgRender!!.native_EglRenderDraw()
        mImageView!!.setImageBitmap(createBitmapFromGLSurface(0, 0, 933, 1400))
    }

    /**
     * 加载 RGBA 图像并传递给本地渲染器。
     * @param resId 资源 ID
     * @param render 渲染器对象
     */
    private fun loadRGBAImage(resId: Int, render: NativeEglRender?) {
        try {
            this.resources.openRawResource(resId).use { inputStream ->
                val bitmap = BitmapFactory.decodeStream(inputStream)
                bitmap?.let {
                    val bytes = it.byteCount
                    val buffer = ByteBuffer.allocateDirect(bytes)
                    it.copyPixelsToBuffer(buffer)
                    buffer.rewind() // 重置缓冲区位置
                    render?.native_EglRenderSetImageData(buffer.array(), it.width, it.height)
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    /**
     * 从 OpenGL 缓冲区中创建位图。
     * @param x 起始 x 坐标
     * @param y 起始 y 坐标
     * @param w 宽度
     * @param h 高度
     * @return 从 OpenGL 缓冲区生成的位图
     */
    private fun createBitmapFromGLSurface(x: Int, y: Int, w: Int, h: Int): Bitmap? {
        val bitmapBuffer = IntArray(w * h)
        val bitmapSource = IntArray(w * h)
        val intBuffer = IntBuffer.wrap(bitmapBuffer)
        intBuffer.position(0)
        try {
            GLES20.glReadPixels(
                x, y, w, h, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE,
                intBuffer
            )
            var offset1: Int
            var offset2: Int
            for (i in 0 until h) {
                offset1 = i * w
                offset2 = (h - i - 1) * w
                for (j in 0 until w) {
                    val texturePixel = bitmapBuffer[offset1 + j]
                    val blue = texturePixel shr 16 and 0xff
                    val red = texturePixel shl 16 and 0x00ff0000
                    val pixel = texturePixel and -0xff0100 or red or blue
                    bitmapSource[offset2 + j] = pixel
                }
            }
        } catch (e: GLException) {
            return null
        }
        return Bitmap.createBitmap(bitmapSource, w, h, Bitmap.Config.ARGB_8888)
    }

    companion object {
        private const val TAG = "EGLActivity" // 日志标记
        const val PARAM_TYPE_SHADER_INDEX = 200 // 着色器索引参数类型
    }
}