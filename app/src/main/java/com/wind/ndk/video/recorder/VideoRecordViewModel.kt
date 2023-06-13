package com.wind.ndk.video.recorder

import android.app.Application
import android.os.Handler
import android.os.Looper
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProvider.AndroidViewModelFactory.Companion.APPLICATION_KEY
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.CreationExtras
import androidx.lifecycle.viewmodel.initializer
import androidx.lifecycle.viewmodel.viewModelFactory
import com.wind.ndk.audio.recorder.AudioRecorder
import com.wind.ndk.camera.CameraPreviewScheduler
import com.wind.ndk.camera.VideoRecorder
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.combine
import kotlinx.coroutines.flow.stateIn
import java.io.File

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
    private val mApp: Application,
    private val mVideoRecorder: VideoRecorder
) : AndroidViewModel(mApp) {
    private val mInteractor=RecordingTimeInteractor()
    private val mHandler = Handler(Looper.getMainLooper())
    private var mSecs = 0

    val state: StateFlow<RecordViewState> = combine(mInteractor.flow){ args: Array<*> ->
        RecordViewState(formattedTime=args[0] as String)
    }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(),
            initialValue = RecordViewState.Empty,
        )



    private var mRecording = false
    fun onClickRecord() {
        if (!mRecording) {
            mSecs = 0
            mRecording = true
            val h264File = "${mApp.getExternalFilesDir(null)?.absolutePath}/record.flv"
            val file = File(h264File)
            if (file.exists()) {
                file.delete()
            }
            mVideoRecorder.startRecord(
                outputUri = h264File,
                videoFrameRate = 24,
                videoBitrate = 1280 * 720 * 2,
                videoWidth = 720,
                videoHeight = 1280,
                audioBitrate = 64 * 1000,
                audioChannels = 1, //录制的时候使用单声道，后期使用ffmpeg转化成双声道
                audioSampleRate = 44100,

                )
            startRecordingTimer()
        } else {
            mVideoRecorder.stop()
            mRecording = false
            mSecs = 0
            mInteractor(mSecs)
            mHandler.removeCallbacksAndMessages(null)
        }

    }

    private fun startRecordingTimer() {
        mSecs = 0
        mInteractor(mSecs)
        runRecordingTimer()
    }



    private val mRunnable = Runnable {
        mSecs++
        runRecordingTimer()
    }

    private fun runRecordingTimer() {
        mInteractor(mSecs)
        mHandler.postDelayed(mRunnable, 1000)
    }


    override fun onCleared() {
        super.onCleared()
        if (mRecording) {
            mVideoRecorder.stop()
            mRecording = false
        }
        mHandler.removeCallbacksAndMessages(null)
    }


    companion object {
        private object CameraPreviewKeyImpl : CreationExtras.Key<CameraPreviewScheduler>

        @JvmField
        val CAMERA_PREVIEW_KEY: CreationExtras.Key<CameraPreviewScheduler> = CameraPreviewKeyImpl

        private object AudioRecorderKeyImpl : CreationExtras.Key<AudioRecorder>

        @JvmField
        val AUDIO_RECORDER_KEY: CreationExtras.Key<AudioRecorder> = AudioRecorderKeyImpl
        val Factory: ViewModelProvider.Factory = viewModelFactory {
            initializer {
                val app = this[APPLICATION_KEY] as Application
                val cameraPreviewScheduler = this[CAMERA_PREVIEW_KEY] as CameraPreviewScheduler
                val audioRecorder = this[AUDIO_RECORDER_KEY] as AudioRecorder
                VideoRecordViewModel(app, VideoRecorder(cameraPreviewScheduler, audioRecorder))
            }
        }
    }
}