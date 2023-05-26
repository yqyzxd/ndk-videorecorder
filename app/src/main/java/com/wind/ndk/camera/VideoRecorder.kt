package com.wind.ndk.camera

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
class VideoRecorder {


    fun startRecord(outputUri:String,
                    videoBitrate:Int,videoWidth:Int,videoHeight:Int,
                    audioBitrate:Int,audioSampleRate:Int,audioChannels:Int){

        thread {
           var ret= startConsumer(outputUri,videoBitrate,videoWidth,videoHeight,audioBitrate,audioSampleRate,audioChannels)
            if (ret<0){
                //error
                return@thread
            }
            startProducer(videoWidth,videoHeight)
        }

    }

    private fun startProducer(videoWidth: Int, videoHeight: Int) :Int{
        return nativeStartProducer(videoWidth,videoHeight)
    }

    private external fun nativeStartProducer(videoWidth: Int, videoHeight: Int): Int

    private fun startConsumer(outputUri:String,
                              videoFrameRate:Int,videoBitrate:Int,videoWidth:Int,videoHeight:Int,
                              audioBitrate:Int,audioSampleRate:Int,audioChannels:Int): Int {
        return nativeStartConsumer(outputUri,videoBitrate,videoWidth,videoHeight,audioBitrate,audioSampleRate,audioChannels)

    }

    private external fun nativeStartConsumer(
        outputUri: String,
        videoFrameRate:Int,
        videoBitrate: Int,
        videoWidth: Int,
        videoHeight: Int,
        audioBitrate: Int,
        audioSampleRate: Int,
        audioChannels: Int
    ): Int


}

