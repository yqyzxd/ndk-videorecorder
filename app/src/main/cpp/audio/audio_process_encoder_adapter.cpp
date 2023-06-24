//
// Created by 史浩 on 2023/6/24.
//

#include "audio_process_encoder_adapter.h"

AudioProcessEncoderAdapter::AudioProcessEncoderAdapter() {
}

AudioProcessEncoderAdapter::~AudioProcessEncoderAdapter() noexcept {

}

int AudioProcessEncoderAdapter::init(int audioBitrate, int audioSampleRate, int audioChannels) {
    int ret=AudioEncoderAdapter::init(audioBitrate,audioSampleRate,audioChannels);
    if (ret<0){
        return -1;
    }
    mAccompanyPool=VideoPacketPool::getInstance();
    mMerger=new MusicMerger();
    return 0;
}
int AudioProcessEncoderAdapter::processAudio() {
    int ret =mPacketBufferSize;

    AudioFrame* accompanyFrame= nullptr;
    if (mAccompanyPool->getAccompanyFrame(&accompanyFrame)<0){
        return -1;
    }

    if (accompanyFrame!= nullptr){
        LOGE("accompanyFrame->size:%d,audioFrame->size:%d",accompanyFrame->size,mPacketBufferSize);
        ret= mMerger->mergeMusic(accompanyFrame->buffer,
                            accompanyFrame->size,
                            mPacketBuffer,
                            mPacketBufferSize);
        delete accompanyFrame;
        accompanyFrame= nullptr;
    }
    return ret;
}

void AudioProcessEncoderAdapter::discardAudioPacket() {
    AudioEncoderAdapter::discardAudioPacket();

    //todo detectDiscardAccompanyFrame
    /*while(mAccompanyPool->detectDiscardAccompanyFrame()){
        if (!mAccompanyPool->discardAccompanyFrame()){
            break;
        }
    }*/
}
void AudioProcessEncoderAdapter::dealloc() {
    AudioEncoderAdapter::dealloc();
}