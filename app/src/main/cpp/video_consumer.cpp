//
// Created by wind on 2023/5/25.
//

#include "video_consumer.h"

VideoConsumer::VideoConsumer() {

}

VideoConsumer::~VideoConsumer() noexcept {

}

int VideoConsumer::getVideoPacketCallback(VideoPacket **pkt, void *ctx) {
    VideoConsumer* consumer= static_cast<VideoConsumer *>(ctx);
    return consumer->getVideoPacket(pkt);
}
int VideoConsumer::getAudioPacketCallback(AudioPacket **pkt, void *ctx) {
    VideoConsumer* consumer= static_cast<VideoConsumer *>(ctx);
    return consumer->getAudioPacket(pkt);
}



int VideoConsumer::init(const char *outputUri, int videoFrameRate, int videoBitrate, int videoWidth,
                        int videoHeight, int audioBitrate, int audioSampleRate, int audioChannels) {


    mStop= false;
    mVideoPacketPool=VideoPacketPool::getInstance();
    mVideoPacketPool->initPool();

    mPublisher=new VideoPublisher();
    mPublisher->setVideoPacketProvider(getVideoPacketCallback,this);
    mPublisher->setAudioPacketProvider(getAudioPacketCallback,this);
    return mPublisher->init(outputUri,videoFrameRate,videoBitrate,videoWidth,videoHeight,audioBitrate,audioSampleRate,audioChannels);

}
void VideoConsumer::run() {
    while (!mStop){
        //LOGI("before publisher encode");
        int ret=mPublisher->encode();
        //LOGI("after publisher encode");
        if (ret<0){
            LOGE("publisher encode error");
            break;
        }
    }

    mPublisher->stop();
    LOGI("VideoConsumer stop run");
}


int VideoConsumer::getVideoPacket(VideoPacket **packet) {
   return mVideoPacketPool->getVideoPacket(packet);
}
int VideoConsumer::getAudioPacket(AudioPacket **packet) {
    return mVideoPacketPool->getAudioPacket(packet);
}
void VideoConsumer::stop() {
    LOGE("enter VideoConsumer stop:%d",mStop);
    if (!mStop){
        mVideoPacketPool->abortVideoPacketQueue();
        mVideoPacketPool->abortAudioPacketQueue();
        mVideoPacketPool->abortAudioFrameQueue();
        mStop= true;
        LOGE("before VideoConsumer stop join");
        join();
        LOGE("after VideoConsumer stop join");
    }
}

void VideoConsumer::dealloc() {
    if (mVideoPacketPool!= nullptr){
        mVideoPacketPool->dealloc();
        delete mVideoPacketPool;
        mVideoPacketPool= nullptr;
    }

    if (mPublisher!= nullptr){
        mPublisher->dealloc();
        delete mPublisher;
        mPublisher= nullptr;
    }

}