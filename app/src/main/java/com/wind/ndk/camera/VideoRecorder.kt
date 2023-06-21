package com.wind.ndk.camera

import android.util.Log
import com.wind.ndk.audio.recorder.AudioRecorder
import com.wind.ndk.audio.recorder.PcmCollector
import kotlin.concurrent.thread

/**
 * FileName: VideoRecorder
 * Author: wind
 * Date: 2023/5/25 14:49
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
class VideoRecorder(
    private val mCameraPreviewScheduler: CameraPreviewScheduler,
    private val mAudioRecorder:AudioRecorder
    ) {


    private var mStop = false
    fun startRecord(
        outputUri: String,
        videoFrameRate: Int,
        videoBitrate: Int, videoWidth: Int, videoHeight: Int,
        audioBitrate: Int, audioSampleRate: Int, audioChannels: Int
    ) {

        thread {
            var ret = startConsumer(
                outputUri,
                videoFrameRate,
                videoBitrate,
                videoWidth,
                videoHeight,
                audioBitrate,
                audioSampleRate,
                2
            )
            if (ret < 0) {
                //error
                return@thread
            }
            startProducer(
                videoFrameRate, videoBitrate, videoWidth, videoHeight,audioBitrate,audioSampleRate,audioChannels
            )


        }

    }

    fun stop() {
        if (!mStop){
            mStop=true
            Log.i("VideoRecorder","mCameraPreviewScheduler.stopEncode")
            mCameraPreviewScheduler.stopEncode()
            Log.e("VideoRecorder","before stopConsumer")
            stopConsumer()
            Log.e("VideoRecorder","after stopConsumer")
        }


    }

    private fun stopConsumer() {
        mAudioRecorder.stop()
        nativeStopConsumer()
    }



    private fun startProducer(
        videoFrameRate: Int,
        videoBitrate: Int,
        videoWidth: Int,
        videoHeight: Int,
        audioBitrate: Int, audioSampleRate: Int, audioChannels: Int
    ): Int {
        //启动preview的encode
        mCameraPreviewScheduler.startEncode(videoWidth, videoHeight, videoBitrate, videoFrameRate,)

        //启动录音
        //计算bufferSizeInShort


        mAudioRecorder.configure(audioSampleRate,audioChannels)
        val collector= PcmCollector(audioSampleRate,mAudioRecorder.getBufferSizeInShort())
        mAudioRecorder.setConsumer(collector)
        mAudioRecorder.start()

        return 0
    }


    private fun startConsumer(
        outputUri: String,
        videoFrameRate: Int, videoBitrate: Int, videoWidth: Int, videoHeight: Int,
        audioBitrate: Int, audioSampleRate: Int, audioChannels: Int
    ): Int {
        return nativeStartConsumer(
            outputUri,
            videoFrameRate,
            videoBitrate,
            videoWidth,
            videoHeight,
            audioBitrate,
            audioSampleRate,
            audioChannels
        )

    }


    private external fun nativeStartConsumer(
        outputUri: String,
        videoFrameRate: Int,
        videoBitrate: Int,
        videoWidth: Int,
        videoHeight: Int,
        audioBitrate: Int,
        audioSampleRate: Int,
        audioChannels: Int
    ): Int

    private external fun nativeStopConsumer()
    fun release() {
        nativeRelease()
    }

    private external fun nativeRelease()
}

