//
// Created by wind on 2023/5/25.
//

#include "video_consumer.h"

VideoConsumer::VideoConsumer() {

}

VideoConsumer::~VideoConsumer() noexcept {

}

int VideoConsumer::init(const char *outputUri, int videoFrameRate, int videoBitrate, int videoWidth,
                        int videoHeight, int audioBitrate, int audioSampleRate, int audioChannels) {



    mVideoPacketPool=VideoPacketPool::getInstance();


}
void VideoConsumer::run() {

}