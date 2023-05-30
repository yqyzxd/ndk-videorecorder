//
// Created by wind on 2023/5/30.
//

#ifndef NDK_VIDEORECORDER_AUDIO_ENCODER_ADAPTER_H
#define NDK_VIDEORECORDER_AUDIO_ENCODER_ADAPTER_H


#include "../utils/thread.h"
#include "audio_frame.h"
#include "../libs/blocking_queue/blocking_queue.h"
#include "audio_encoder.h"

typedef int (*PcmPacketProvider)(AudioPacket**);
class AudioEncoderAdapter : public Thread{

public:
    AudioEncoderAdapter();
    ~AudioEncoderAdapter();

    int init(int audioBitrate,int audioSampleRate, int audioChannels);

    void setPcmPacketProvider(PcmPacketProvider provider,void* ctx);


    void run();

    void dealloc();

private:
    int mPacketBufferSize;
    short* mPacketBuffer;
    int mPacketBufferCursor;

    int mAudioChannels;
    int mAudioBitrate;
    int mAudioSampleRate;

    bool mRunning;

    AudioEncoder* mAudioEncoder;
    BlockingQueue<AudioPacket*>* mAACQueue;
};


#endif //NDK_VIDEORECORDER_AUDIO_ENCODER_ADAPTER_H
