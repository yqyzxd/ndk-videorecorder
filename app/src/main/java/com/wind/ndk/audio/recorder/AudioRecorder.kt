package com.wind.ndk.audio.recorder

import android.annotation.SuppressLint
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import com.wind.ndk.audiorecoder.AudioConfigurationException

object AudioRecorder {
    private const val SAMPLE_RATE_IN_HZ_44100=44100
    private const val SAMPLE_RATE_IN_HZ_16000=16000
    private var mChannelConfiguration=AudioFormat.CHANNEL_IN_MONO
    private const val AUDIO_FORMAT=AudioFormat.ENCODING_PCM_16BIT
    private const val AUDIO_SOURCE=MediaRecorder.AudioSource.MIC

    private var mAudioRecord :AudioRecord?=null
    private var mBufferSizeInBytes=0
    private var mBufferSizeInShort=0
    private var mRecording = false
    private var mRecordThread:Thread?=null
    private var mConsumer:Consumer?=null

    @SuppressLint("MissingPermission")
    @Throws(AudioConfigurationException::class,AudioRecordPermissionDeniedException::class)
    fun configure(/*context:Context*/audioSampleRate: Int, audioChannels: Int){

        /*if (ActivityCompat.checkSelfPermission(context,android.Manifest.permission.RECORD_AUDIO)!= PackageManager.PERMISSION_GRANTED){
            throw AudioRecordPermissionDeniedException()
        }*/

        mAudioRecord?.release()

        val sampleRateInHzs= listOf(audioSampleRate)//listOf(SAMPLE_RATE_IN_HZ_44100,SAMPLE_RATE_IN_HZ_16000)
        mChannelConfiguration = if (audioChannels==1) AudioFormat.CHANNEL_IN_MONO else AudioFormat.CHANNEL_IN_STEREO

        for (sampleRateInHz in sampleRateInHzs){
            try {
                mBufferSizeInBytes=AudioRecord.getMinBufferSize(sampleRateInHz,mChannelConfiguration,AUDIO_FORMAT)
                mAudioRecord= AudioRecord(AUDIO_SOURCE, sampleRateInHz, mChannelConfiguration,AUDIO_FORMAT,mBufferSizeInBytes)

            }catch (e:Exception){
                throw AudioRecordPermissionDeniedException()
            }

            if (mAudioRecord?.state == AudioRecord.STATE_INITIALIZED){
                break
            }

            //创建AudioRecord失败，更换采样率重试
        }

        mBufferSizeInShort = mBufferSizeInBytes/2
        if (mAudioRecord?.state != AudioRecord.STATE_INITIALIZED){
            //还是失败
            throw AudioConfigurationException()
        }

    }

    fun getBufferSizeInShort() = mBufferSizeInBytes

    fun start(){
        if (mAudioRecord?.state == AudioRecord.STATE_INITIALIZED){
            mAudioRecord?.startRecording()

            mRecording=true

            mRecordThread= Thread(RecordRunnable())
            mRecordThread?.start()
        }
    }

    fun stop(){
        mRecording=false
        mRecordThread?.join()
        //release里面已经调用了stop方法
        mAudioRecord?.release()
        mAudioRecord=null

    }

    private class RecordRunnable:Runnable{
        override fun run() {
           val samples= ShortArray(mBufferSizeInShort)
            mConsumer?.begin()
            while (mRecording){
                val audioSampleSize= mAudioRecord?.read(samples,0, mBufferSizeInShort)?:0
                if (audioSampleSize>0){
                    mConsumer?.consume(samples,audioSampleSize)
                }
            }
            mConsumer?.end()

        }

    }

    fun setConsumer(consumer: Consumer){
        this.mConsumer=consumer
    }
    interface Consumer{
        fun begin()
        fun consume(shortArray: ShortArray, sizeInShort:Int)
        fun end()
    }

}