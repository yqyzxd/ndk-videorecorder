//
// Created by wind on 2023/5/30.
//

#include "audio_encoder_adapter.h"

#define LOG_TAG "AudioEncoderAdapter"
AudioEncoderAdapter::AudioEncoderAdapter() {


}

AudioEncoderAdapter::~AudioEncoderAdapter() noexcept {

}

int AudioEncoderAdapter::provideAudioFrameCallback(short *samples, int frameSize, int nbChannels,
                                                   int64_t *pts,
                                               void *ctx) {
    AudioEncoderAdapter *adapter = static_cast<AudioEncoderAdapter *>(ctx);
    return adapter->provideAudioFrame(samples, frameSize, nbChannels, pts);

}

int AudioEncoderAdapter::audioPacketCollector(AudioPacket *packet,void* ctx) {
    AudioEncoderAdapter* adapter= static_cast<AudioEncoderAdapter *>(ctx);
    return adapter->collectAudioPacket(packet);
}

int AudioEncoderAdapter::init(int audioBitrate, int audioSampleRate, int audioChannels) {

    this->mAudioBitrate = audioBitrate;
    this->mAudioSampleRate = audioSampleRate;
    this->mAudioChannels = audioChannels;
    /** iOS是1.0f Android是2.0f **/
   // mChannelRatio = 1.0f;

    mPacketBufferCursor=0;
    mPacketBufferSize=0;
    mPacketBufferPresentationTimeMillis=0;
    mPacketBuffer= nullptr;

    mRunning= true;
    mAudioPool = VideoPacketPool::getInstance();
    start();

    return 0;
}

int
AudioEncoderAdapter::provideAudioFrame(short *samples, int frameSize, int nbChannels, int64_t *pts) {
    int sizeInByte = frameSize * nbChannels * 2;

    int sampleCursorInShort = 0;
    while (true) {
        if (mPacketBufferSize == 0) {
            int ret = getAudioPacket();
            if (ret < 0) {
                return ret;
            }
        }

        int cpySizeInShort = (sizeInByte - sampleCursorInShort * 2) / 2;
        if (mPacketBufferCursor + cpySizeInShort <= mPacketBufferSize) {
            cpyToSamples(samples, sampleCursorInShort, cpySizeInShort, pts);
            mPacketBufferCursor += cpySizeInShort;
            sampleCursorInShort = 0;
            break;
        } else {
            int sub = mPacketBufferSize - mPacketBufferCursor;
            cpyToSamples(samples, sampleCursorInShort, sub, pts);
            sampleCursorInShort += sub;
            mPacketBufferSize = 0;
            continue;
        }
    }
    return frameSize * nbChannels;


}

void AudioEncoderAdapter::run() {
    mAudioEncoder = new AudioEncoder();

    mAudioEncoder->init(mAudioBitrate, mAudioChannels, mAudioSampleRate, 16, "libfdk_aac");
    mAudioEncoder->setAudioFrameProvider(provideAudioFrameCallback, this);
    mAudioEncoder->setAudioPacketCollector(audioPacketCollector,this);
    while (mRunning) {
        //LOGI("before audio encoder adapter run");
        mAudioEncoder->encode();
        //LOGI("after audio encoder adapter run");
    }
    mAudioEncoder->destroy();
}

void AudioEncoderAdapter::dealloc() {
    mRunning = false;
    mAudioPool->abortAudioFrameQueue();
    //LOGI("before encoder adapter dealloc join");
    join();
    //LOGI("after encoder adapter dealloc join");
    if (mPacketBuffer != nullptr) {
        delete[] mPacketBuffer;
        mPacketBuffer = nullptr;
    }

}

int AudioEncoderAdapter::getAudioPacket() {
    LOGI("before discardAudioPacket");
    discardAudioPacket();
    AudioFrame* pkt;
    LOGI("before get audio packet");
    int ret = mAudioPool->getAudioFrame(&pkt);
    LOGI("after get audio packet:ret %d",ret);
    if(ret<0){
        return -1;
    }

    mPacketBufferCursor=0;
    mPacketBufferPresentationTimeMillis=pkt->position;
    /**
	 * 在Android平台 录制是单声道的 经过音效处理之后是双声道 channelRatio 2
	 * 在iOS平台 录制的是双声道的 是已经处理音效过后的 channelRatio 1
	 */
    mPacketBufferSize=pkt->size /* *mChannelRatio*/;
    if (mPacketBuffer== nullptr){
        mPacketBuffer=new short[mPacketBufferSize];
    }
    memcpy(mPacketBuffer,pkt->buffer,pkt->size*sizeof(short));


    delete pkt;
    pkt= nullptr;
    return 0;
}

int AudioEncoderAdapter::cpyToSamples(short *samples, int samplesCursorInShort, int cpySizeInShort,
                                      int64_t *pts) {
    if (samplesCursorInShort == 0) {

        double duration =
                mPacketBufferCursor * 1000.0f / (double) (mAudioSampleRate /* * mChannelRatio*/);
        *pts = mPacketBufferPresentationTimeMillis + duration;
    }
    memcpy(samples + samplesCursorInShort, mPacketBuffer + mPacketBufferCursor,
           cpySizeInShort * sizeof(short));
    return 1;
}

void AudioEncoderAdapter::discardAudioPacket() {

    while (mAudioPool->detectDiscardAudioFrame()) {
        LOGI("detectDiscardAudioFrame");
        if (!mAudioPool->discardAudioFrame()){
            break;
        }
    }
}

int AudioEncoderAdapter::collectAudioPacket(AudioPacket *packet) {

    int ret=  mAudioPool->enqueueAudioPacket(packet);
    //LOGI("audio packet size:%d",mAudioPool->getVideoPacketQueueSize());
    return ret;
}