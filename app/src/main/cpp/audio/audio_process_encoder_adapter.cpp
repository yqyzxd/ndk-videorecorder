//
// Created by 史浩 on 2023/6/24.
//

#include "audio_process_encoder_adapter.h"

AudioProcessEncoderAdapter::AudioProcessEncoderAdapter() {
}

AudioProcessEncoderAdapter::~AudioProcessEncoderAdapter() noexcept {

}

int AudioProcessEncoderAdapter::init(int audioBitrate, int audioSampleRate, int audioChannels) {
    int ret = AudioEncoderAdapter::init(audioBitrate, audioSampleRate, audioChannels);
    if (ret < 0) {
        return -1;
    }
    mAccompanyCursor=0;
    mAccompanyPool = VideoPacketPool::getInstance();
    mMerger = new MusicMerger();
    return 0;
}

int AudioProcessEncoderAdapter::processAudio() {
    //int ret =mPacketBufferSize;

    if (mAccompanyFrame == nullptr) {
        if (mAccompanyPool->getAccompanyFrameQueueSize() > 0) {
            if (mAccompanyPool->getAccompanyFrame(&mAccompanyFrame) < 0) {
                return mPacketBufferSize;
            }
        }
    }

    if (mAccompanyFrame != nullptr) {
        int audioSamplesCursor = 0;
        int ret;//-2 means more
        bool more = true;
        while (more) {
            ret = mMerger->mergeMusic(mAccompanyFrame->buffer,
                                      mAccompanyFrame->size,
                                      &mAccompanyCursor,
                                      mPacketBuffer,
                                      mPacketBufferSize,
                                      &audioSamplesCursor);
            LOGE("mAccompanyCursor:%d,audioSamplesCursor:%d,queueSize:%d", mAccompanyCursor, audioSamplesCursor,mAccompanyPool->getAccompanyFrameQueueSize());
            more = (ret == -2);
            if (more) {
                LOGE("mAccompanyPool->getAccompanyFrameQueueSize():%d",mAccompanyPool->getAccompanyFrameQueueSize());
                if (mAccompanyPool->getAccompanyFrameQueueSize() > 0) {
                    AudioFrame* audioFrame;
                    if (mAccompanyPool->getAccompanyFrame(&audioFrame) < 0) {
                        more = false;
                    } else{
                        delete mAccompanyFrame;
                        mAccompanyCursor=0;
                        mAccompanyFrame=audioFrame;
                    }
                } else {
                    more= false;
                }
                if (!more){
                    delete mAccompanyFrame;
                    mAccompanyFrame= nullptr;
                    mAccompanyCursor=0;
                }
            }

        }


    }
    return mPacketBufferSize;
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
    if (mAccompanyFrame!= nullptr){
        delete mAccompanyFrame;
        mAccompanyFrame= nullptr;
    }
    mAccompanyCursor=0;
    AudioEncoderAdapter::dealloc();
}