//
// Created by wind on 2023/5/30.
//

#include "audio_encoder_adapter.h"
AudioEncoderAdapter::AudioEncoderAdapter() {

}

AudioEncoderAdapter::~AudioEncoderAdapter() noexcept {

}

int AudioEncoderAdapter::init(int audioBitrate, int audioSampleRate, int audioChannels) {

    this->mAudioBitrate=audioBitrate;
    this->mAudioSampleRate=audioSampleRate;
    this->mAudioChannels=audioChannels;


    mAACQueue=new LinkedBlockingQueue<AudioPacket*>();

    start();
}


void AudioEncoderAdapter::run() {
    mAudioEncoder=new AudioEncoder();

    mAudioEncoder->init(mAudioBitrate,mAudioChannels,mAudioSampleRate,16,"fdk_aac");
    while (mRunning){
        AudioPacket* audioPacket;
       int ret= mAudioEncoder->encode(&audioPacket);
    }
    mAudioEncoder->destroy();
}

void AudioEncoderAdapter::dealloc() {
    mRunning= false;
    mAACQueue->flush();
    join();

    if (mPacketBuffer!= nullptr){
        delete[] mPacketBuffer;
        mPacketBuffer= nullptr;
    }

}