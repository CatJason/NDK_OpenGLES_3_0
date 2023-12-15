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

class AudioCollector : OnRecordPositionUpdateListener {
    private var mAudioRecord: AudioRecord? = null
    private var mThread: Thread? = null
    private var mAudioBuffer: ShortArray? = null
    private var mHandler: Handler? = null
    private var mBufferSize: Int = 2 * AudioRecord.getMinBufferSize(
        RECORDER_SAMPLE_RATE,
        RECORDER_CHANNELS, RECORDER_AUDIO_ENCODING
    )
    private var mCallback: Callback? = null

    fun init(context: Context) {
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.RECORD_AUDIO
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            return
        }
        mAudioRecord = AudioRecord(
            MediaRecorder.AudioSource.MIC,
            RECORDER_SAMPLE_RATE,
            RECORDER_CHANNELS,
            RECORDER_AUDIO_ENCODING,
            mBufferSize
        )
        mAudioRecord?.startRecording()
        mThread = object : Thread("Audio-Recorder") {
            override fun run() {
                super.run()
                val audioBuffer = ShortArray(mBufferSize)
                mAudioBuffer = audioBuffer
                Looper.prepare()
                val looper = Looper.myLooper()?: return
                mHandler = Handler(looper)
                mAudioRecord?.setRecordPositionUpdateListener(this@AudioCollector, mHandler)
                val bytePerSample = RECORDER_ENCODING_BIT / 8
                val samplesToDraw = (mBufferSize / bytePerSample).toFloat()
                mAudioRecord?.positionNotificationPeriod = samplesToDraw.toInt()
                mAudioRecord?.read(audioBuffer, 0, mBufferSize)
                Looper.loop()
            }
        }
        mThread?.start()
    }

    fun unInit() {
        mAudioRecord?.stop()
        mAudioRecord?.release()
        mHandler?.looper?.quitSafely()
        mAudioRecord = null
    }

    fun addCallback(callback: Callback?) {
        mCallback = callback
    }

    override fun onMarkerReached(recorder: AudioRecord) {}
    override fun onPeriodicNotification(recorder: AudioRecord) {
        val audioRecord = mAudioRecord?: return
        val audioBuffer = mAudioBuffer?: return
        if (audioRecord.recordingState == AudioRecord.RECORDSTATE_RECORDING && audioRecord.read(
                audioBuffer,
                0,
                audioBuffer.size
            ) != -1
        ) {
            mCallback?.onAudioBufferCallback(audioBuffer)
        }
    }

    interface Callback {
        fun onAudioBufferCallback(buffer: ShortArray?)
    }

    companion object {
        private const val TAG = "AudioRecorderWrapper"
        private const val RECORDER_SAMPLE_RATE = 44100
        private const val RECORDER_CHANNELS = 1
        private const val RECORDER_ENCODING_BIT = 16
        private const val RECORDER_AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT
    }
}