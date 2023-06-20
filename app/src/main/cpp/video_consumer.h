//
// Created by wind on 2023/5/25.
//

#ifndef NDK_VIDEORECORDER_VIDEO_CONSUMER_H
#define NDK_VIDEORECORDER_VIDEO_CONSUMER_H


#include "utils/thread.h"
#include "video_packet_pool.h"
#include "video_publisher.h"

class VideoConsumer : public Thread{
public:
    VideoConsumer();
    ~VideoConsumer();

    int init(const char* outputUri,int videoFrameRate,int videoBitrate,int videoWidth,int videoHeight,
             int audioBitrate,int audioSampleRate,int audioChannels);


    void run() override;

    void stop();
    void dealloc();


    static int getVideoPacketCallback(VideoPacket** pkt,void* ctx);
    static int getAudioPacketCallback(AudioPacket** pkt,void* ctx);


private:
    VideoPacketPool* mVideoPacketPool= nullptr;
    VideoPublisher* mPublisher;


    bool mStop;

    int getVideoPacket(VideoPacket **packet);
    int getAudioPacket(AudioPacket **packet);
};


#endif //NDK_VIDEORECORDER_VIDEO_CONSUMER_H
