//
// Created by wind on 2023/5/25.
//

#ifndef NDK_VIDEORECORDER_VIDEO_CONSUMER_H
#define NDK_VIDEORECORDER_VIDEO_CONSUMER_H


#include "utils/thread.h"
#include "video_packet_pool.h"

class VideoConsumer : public Thread{
public:
    VideoConsumer();
    ~VideoConsumer();

    int init(const char* outputUri,int videoFrameRate,int videoBitrate,int videoWidth,int videoHeight,
             int audioBitrate,int audioSampleRate,int audioChannels);


    void run() override;

private:
    VideoPacketPool* mVideoPacketPool= nullptr;
};


#endif //NDK_VIDEORECORDER_VIDEO_CONSUMER_H
