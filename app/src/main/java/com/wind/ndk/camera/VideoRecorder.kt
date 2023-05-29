package com.wind.ndk.camera

import com.wind.ndk.audio.recorder.AudioRecorder
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
                audioChannels
            )
            if (ret < 0) {
                //error
                return@thread
            }
            startProducer(
                videoFrameRate, videoBitrate, videoWidth, videoHeight
            )
        }

    }

    fun stop() {
        mCameraPreviewScheduler.stopEncode()
        stopConsumer()

    }

    private fun stopConsumer() {
        nativeStopConsumer()
    }

    private external fun nativeStopConsumer()

    private fun startProducer(
        videoFrameRate: Int,
        videoBitrate: Int,
        videoWidth: Int,
        videoHeight: Int,
    ): Int {
        //启动preview的encode
        mCameraPreviewScheduler.startEncode(videoWidth, videoHeight, videoBitrate, videoFrameRate)

        //启动录音
        mAudioRecorder.configure()
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


}
