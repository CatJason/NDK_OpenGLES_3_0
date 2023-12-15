package com.byteflow.app.audio

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.AudioRecord.OnRecordPositionUpdateListener
import android.media.MediaRecorder
import android.os.Handler
import android.os.Looper
import androidx.core.app.ActivityCompat

/**
 * 音频采集器类，用于从麦克风捕获音频数据。
 */
class AudioCollector : OnRecordPositionUpdateListener {
    private var mAudioRecord: AudioRecord? = null // 音频录制对象
    private var mThread: Thread? = null // 采集线程
    private var mAudioBuffer: ShortArray? = null // 音频缓冲区
    private var mHandler: Handler? = null // 主线程处理器
    private var mBufferSize: Int = 2 * AudioRecord.getMinBufferSize(
        RECORDER_SAMPLE_RATE,
        RECORDER_CHANNELS, RECORDER_AUDIO_ENCODING
    ) // 音频缓冲区大小
    private var mCallback: Callback? = null // 回调接口

    /**
     * 初始化音频采集器。
     * @param context 应用程序上下文，用于权限检查。
     */
    fun init(context: Context) {
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.RECORD_AUDIO
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            return // 如果没有录音权限，则直接返回
        }
        // 创建 AudioRecord 实例
        mAudioRecord = AudioRecord(
            MediaRecorder.AudioSource.MIC,
            RECORDER_SAMPLE_RATE,
            RECORDER_CHANNELS,
            RECORDER_AUDIO_ENCODING,
            mBufferSize
        )
        mAudioRecord?.startRecording() // 开始录音
        mThread = object : Thread("Audio-Recorder") {
            override fun run() {
                super.run()
                val audioBuffer = ShortArray(mBufferSize) // 创建音频缓冲区
                mAudioBuffer = audioBuffer
                Looper.prepare() // 初始化线程的 Looper
                val looper = Looper.myLooper() ?: return // 获取当前线程的 Looper
                mHandler = Handler(looper) // 创建主线程处理器
                mAudioRecord?.setRecordPositionUpdateListener(this@AudioCollector, mHandler) // 设置监听器
                val bytePerSample = RECORDER_ENCODING_BIT / 8 // 每采样字节数
                val samplesToDraw = (mBufferSize / bytePerSample).toFloat() // 计算采样数
                mAudioRecord?.positionNotificationPeriod = samplesToDraw.toInt() // 设置通知周期
                mAudioRecord?.read(audioBuffer, 0, mBufferSize) // 开始读取音频数据
                Looper.loop() // 开始消息循环
            }
        }
        mThread?.start() // 启动采集线程
    }

    /**
     * 释放资源并停止音频采集。
     */
    fun unInit() {
        mAudioRecord?.stop() // 停止录音
        mAudioRecord?.release() // 释放 AudioRecord 资源
        mHandler?.looper?.quitSafely() // 停止 Looper
        mAudioRecord = null
    }

    /**
     * 添加音频数据回调接口。
     * @param callback 回调接口实例。
     */
    fun addCallback(callback: Callback?) {
        mCallback = callback
    }

    /**
     * 当到达标记点时调用（未使用）。
     */
    override fun onMarkerReached(recorder: AudioRecord) {}

    /**
     * 当周期性通知触发时调用。
     */
    override fun onPeriodicNotification(recorder: AudioRecord) {
        val audioRecord = mAudioRecord ?: return
        val audioBuffer = mAudioBuffer ?: return
        if (audioRecord.recordingState == AudioRecord.RECORDSTATE_RECORDING && audioRecord.read(
                audioBuffer,
                0,
                audioBuffer.size
            ) != -1
        ) {
            mCallback?.onAudioBufferCallback(audioBuffer) // 将音频缓冲区回调到调用者
        }
    }

    /**
     * 音频采集回调接口。
     */
    interface Callback {
        /**
         * 当音频缓冲区数据可用时调用。
         * @param buffer 音频缓冲区数据。
         */
        fun onAudioBufferCallback(buffer: ShortArray?)
    }

    companion object {
        private const val TAG = "AudioRecorderWrapper" // 日志标记
        private const val RECORDER_SAMPLE_RATE = 44100 // 采样率
        private const val RECORDER_CHANNELS = 1 // 通道数
        private const val RECORDER_ENCODING_BIT = 16 // 位深
        private const val RECORDER_AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT // 音频格式
    }
}
