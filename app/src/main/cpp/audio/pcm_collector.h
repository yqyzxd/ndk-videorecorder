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
    void stop();
private:
    VideoPacketPool* mPool;
    AudioEncoderAdapter* mAudioEncoderAdapter;

    int mAudioSampleRate=0;
    //每个packet的大小
    int mBufferSizeInShort=0;
    //每个packet的数据中转站
    short* mAudioBuffer= nullptr;
    //当前buffer拷贝了多少个数据
    int mBufferCursor =0;

    int64_t mStartTimeMillis=0;



    void enqueue();


};


#endif //NDK_VIDEORECORDER_PCM_COLLECTOR_H
