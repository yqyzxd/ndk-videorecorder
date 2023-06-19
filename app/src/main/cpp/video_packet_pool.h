//
// Created by wind on 2023/5/24.
//

#ifndef NDK_VIDEORECORDER_VIDEO_PACKET_POOL_H
#define NDK_VIDEORECORDER_VIDEO_PACKET_POOL_H


#include "encoder/video_frame.h"
#include "libs/blocking_queue/blocking_queue.h"
#include "utils/log.h"
#include "audio/audio_frame.h"


#define THRESHOLD_VIDEO_PACKET_QUEUE 60
#define AUDIO_PACKET_DURATION_IN_SECS 0.04f
#define LOG_TAG "VideoPacketPool"
class VideoPacketPool {
public:
    VideoPacketPool();
    ~VideoPacketPool();
    static VideoPacketPool* instance;//单例模式

    int enqueueVideoPacket(VideoPacket* packet);
    int getVideoPacketQueueSize();
    int abortVideoPacketQueue();
    int getVideoPacket(VideoPacket** packet);



    int enqueueAudioFrame(AudioFrame* packet);
    int getAudioFrameQueueSize();
    int abortAudioFrameQueue();
    int getAudioFrame(AudioFrame** packet);


    int enqueueAudioPacket(AudioPacket* packet);
    int getAudioPacketQueueSize();
    int abortAudioPacketQueue();
    int getAudioPacket(AudioPacket** packet);

    static VideoPacketPool* getInstance();

    bool detectDiscardAudioFrame();

    bool discardAudioFrame();


    bool dealloc();



private:
    BlockingQueue<VideoPacket*> *mVideoPktQueue = 0;
    VideoPacket* mCurVideoPacket=0;

    BlockingQueue<AudioFrame*> *mAudioFrameQueue = 0;

    BlockingQueue<AudioPacket*> *mAudioPktQueue = 0;

    int mTotalDiscardVideoPacketDuration;
    pthread_rwlock_t mRWLock;


    bool detectDiscardVideoPacket();
    int discardVideoGOP(int *countOfDiscardPackets, int *durationOfDiscardPackets);

    void recordDropVideoFrame(int discardDuration);

    void initPool();
};


#endif //NDK_VIDEORECORDER_VIDEO_PACKET_POOL_H
