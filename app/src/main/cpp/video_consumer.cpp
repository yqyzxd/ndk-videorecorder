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


    mStop= false;
    mVideoPacketPool=VideoPacketPool::getInstance();
    mPublisher=new VideoPublisher();
    mPublisher->setVideoPacketProvider(getVideoPacketCallback,this);
    mPublisher->setAudioPacketProvider(getAudioPacketCallback,this);
    return mPublisher->init(outputUri,videoFrameRate,videoBitrate,videoWidth,videoHeight,audioBitrate,audioSampleRate,audioChannels);

}
void VideoConsumer::run() {
    while (!mStop){
        //LOGI("VideoConsumer run");
        int ret=mPublisher->encode();
        if (ret<0){
            //error
            break;
        }
    }
    LOGI("VideoConsumer stop run");
    mPublisher->stop();
}

void VideoConsumer::stop() {
    mVideoPacketPool->abortVideoPacketQueue();
    mStop= true;
    LOGI("VideoConsumer stop");
    //join();
}
int VideoConsumer::getVideoPacket(VideoPacket **packet) {
   return mVideoPacketPool->getVideoPacket(packet);
}
