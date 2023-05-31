//
// Created by wind on 2023/5/30.
//

#ifndef NDK_VIDEORECORDER_PCM_COLLECTOR_H
#define NDK_VIDEORECORDER_PCM_COLLECTOR_H


#include "../libs/blocking_queue/blocking_queue.h"
#include "../video_packet_pool.h"
#include "../utils/time.h"
#include "audio_encoder_adapter.h"

class PcmCollector {

public:
    PcmCollector();
    ~PcmCollector();
    int init(int audioSampleRate, int bufferSizeInShort);

    void collect(short *data, int sizeInShort);
private:
    VideoPacketPool* mPool;
    AudioEncoderAdapter* mAudioEncoderAdapter;

    int mAudioSampleRate;
    //每个packet的大小
    int mBufferSizeInShort;
    //每个packet的数据中转站
    short* mAudioBuffer;
    //当前buffer拷贝了多少个数据
    int mBufferCursor;

    long mStartTimeMillis;



    void enqueue();

    void stop();
};


#endif //NDK_VIDEORECORDER_PCM_COLLECTOR_H
