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

int VideoConsumer::getAudioPacket(AudioPacket **packet) {
   return mVideoPacketPool->getAudioPacket(packet);
}

int VideoConsumer::init(const char *outputUri, int videoFrameRate, int videoBitrate, int videoWidth,
                        int videoHeight, int audioBitrate, int audioSampleRate, int audioChannels) {

    av_packet_rescale_ts
    mStop= false;
    mVideoPacketPool=VideoPacketPool::getInstance();
    mPublisher=new VideoPublisher();
    mPublisher->setVideoPacketProvider(getVideoPacketCallback,this);
    mPublisher->setAudioPacketProvider(getAudioPacketCallback,this);
    return mPublisher->init(outputUri,videoFrameRate,videoBitrate,videoWidth,videoHeight,audioBitrate,audioSampleRate,audioChannels);

}
void VideoConsumer::run() {
    while (!mStop){
        LOGI("before publisher encode");
        int ret=mPublisher->encode();
        LOGI("after publisher encode");
        if (ret<0){
            //error
            break;
        }
    }
    LOGI("VideoConsumer stop run");
    mPublisher->stop();
}

void VideoConsumer::stop() {
    if (!mStop){
        mVideoPacketPool->abortVideoPacketQueue();
        mVideoPacketPool->abortAudioPacketQueue();
        mStop= true;
        join();
    }

}
int VideoConsumer::getVideoPacket(VideoPacket **packet) {
   return mVideoPacketPool->getVideoPacket(packet);
}
