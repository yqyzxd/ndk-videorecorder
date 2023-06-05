package com.wind.ndk.video.recorder

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProvider.AndroidViewModelFactory.Companion.APPLICATION_KEY
import androidx.lifecycle.viewmodel.CreationExtras
import androidx.lifecycle.viewmodel.initializer
import androidx.lifecycle.viewmodel.viewModelFactory
import com.wind.ndk.audio.recorder.AudioRecorder
import com.wind.ndk.camera.CameraPreviewScheduler
import com.wind.ndk.camera.VideoRecorder

/**
 * FileName: VideoRecordViewModel
 * Author: wind
 * Date: 2023/5/28 11:46
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
class VideoRecordViewModel(
    private val mApp:Application,
    private val mVideoRecorder: VideoRecorder
): AndroidViewModel(mApp) {

    private var mRecording=false
    fun onClickRecord(){
        if (!mRecording){
            mRecording=true
            val h264File="${mApp.getExternalFilesDir(null)?.absolutePath}/record.flv"
            mVideoRecorder.startRecord(
                outputUri = h264File,
                videoFrameRate = 24,
                videoBitrate = 1280*720*4,
                videoWidth = 1280,
                videoHeight = 720,
                audioBitrate = 64*1000,
                audioChannels = 1, //录制的时候使用单声道，后期使用ffmpeg转化成双声道
                audioSampleRate = 44100,

            )
        }else{
            mVideoRecorder.stop()
            mRecording=false
        }

    }


    override fun onCleared() {
        super.onCleared()
        if (mRecording){
            mVideoRecorder.stop()
            mRecording=false
        }
    }


    companion object{
        private object CameraPreviewKeyImpl : CreationExtras.Key<CameraPreviewScheduler>
        @JvmField
        val CAMERA_PREVIEW_KEY: CreationExtras.Key<CameraPreviewScheduler> =CameraPreviewKeyImpl
        private object AudioRecorderKeyImpl : CreationExtras.Key<AudioRecorder>
        @JvmField
        val AUDIO_RECORDER_KEY: CreationExtras.Key<AudioRecorder> =AudioRecorderKeyImpl
        val Factory:ViewModelProvider.Factory= viewModelFactory {
            initializer {
                val app=this[APPLICATION_KEY] as Application
                val cameraPreviewScheduler=this[CAMERA_PREVIEW_KEY] as CameraPreviewScheduler
                val audioRecorder=this[AUDIO_RECORDER_KEY] as AudioRecorder
                VideoRecordViewModel( app,VideoRecorder(cameraPreviewScheduler,audioRecorder))
            }
        }
    }
}