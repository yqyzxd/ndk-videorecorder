//
// Created by wind on 2023/5/30.
//

#ifndef NDK_VIDEORECORDER_AUDIO_ENCODER_ADAPTER_H
#define NDK_VIDEORECORDER_AUDIO_ENCODER_ADAPTER_H


#include "../utils/thread.h"
#include "audio_frame.h"
#include "../libs/blocking_queue/blocking_queue.h"
#include "audio_encoder.h"
#include "../video_packet_pool.h"


class AudioEncoderAdapter : public Thread{

public:
    AudioEncoderAdapter();
    ~AudioEncoderAdapter();

    int init(int audioBitrate,int audioSampleRate, int audioChannels);




    void run() override;

    void dealloc();

    static int provideAudioFrameCallback(short* samples,int frameSize,int nbChannels,double* pts,void* ctx);
    static int audioPacketCollector(AudioPacket* packet,void* ctx);
private:
    int mPacketBufferSize;
    short* mPacketBuffer;
    int mPacketBufferCursor;
    double mPacketBufferPresentationTimeMillis;

    int mAudioChannels;
    int mAudioBitrate;
    int mAudioSampleRate;
    float mChannelRatio;


    bool mRunning;

    AudioEncoder* mAudioEncoder;

    VideoPacketPool* mAudioPool;
private:
    int provideAudioFrame(short* samples,int frameSize,int nbChannels,double* pts);
    int collectAudioPacket(AudioPacket* packet);

    int getAudioPacket();

    int cpyToSamples(short *samples, int samplesCursorInShort, int cpySizeInShort, double *pts);

    void discardAudioPacket();
};


#endif //NDK_VIDEORECORDER_AUDIO_ENCODER_ADAPTER_H
