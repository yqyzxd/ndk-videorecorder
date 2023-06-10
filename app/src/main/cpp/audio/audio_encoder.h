//
// Created by wind on 2023/4/24.
//

#ifndef NDK_AUDIORECODER_AUDIO_ENCODER_H
#define NDK_AUDIORECODER_AUDIO_ENCODER_H


#include "../utils/types.h"
#include "../utils/log.h"
#include "../utils/time.h"
#include "audio_frame.h"

extern "C"{
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
};



#define BITE_RATE 64000

typedef int (*AudioFrameProvider)(short* samples,int nbSamples,int channels,double * pts,void* ctx);
typedef int (*AudioPacketCollector)(AudioPacket *, void *);
class AudioEncoder {
private:

    AVCodecContext* avCodecContext;
    SwrContext* swrContext;
    AVFrame* inputFrame;
    int nbSamples;

    int bufferSize;
    uint8_t* samples;

    int64_t mAudioNextPts;

    int swrBufferSize;
    uint8_t* swrBuffer;
    AVFrame* swrFrame;

    //音频时长
    double duration;
    double totalSWRTimeMills;

    uint8_t** convertData;

    //通道数
    int mChannels;
    //采样率
    int mSampleRate;
    //码率
    int mBitRate;



    AudioFrameProvider mAudioFrameProvider;
    void* mAudioFrameProviderCtx;

    AudioPacketCollector mAudioPacketCollector;
    void* mAudioPacketCollectorCtx;

public:
    int init(int bitRate, int channels ,int sampleRate,int bitsPerSample,const char* codecName);
    int encode();
    void destroy();

    void setAudioFrameProvider(AudioFrameProvider provider,void* ctx);
    void setAudioPacketCollector(AudioPacketCollector collector, void *ctx);

private:
    int allocAudioStream(const char *codecName);
    int allocAvFrame();

    int encodePacket();
    void addADTSToPacket(uint8_t *buffer, int size);
};


#endif //NDK_AUDIORECODER_AUDIO_ENCODER_H
