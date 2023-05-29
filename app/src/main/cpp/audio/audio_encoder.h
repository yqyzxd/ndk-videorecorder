//
// Created by wind on 2023/4/24.
//

#ifndef NDK_AUDIORECODER_AUDIO_ENCODER_H
#define NDK_AUDIORECODER_AUDIO_ENCODER_H
#include "types.h"
#include "log.h"
#include "utils.h"



extern "C"{
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
};



#define BITE_RATE 64000

class AudioEncoder {
private:
    AVFormatContext* avFormatContext;
    AVStream* audioStream;
    AVCodecContext* avCodecContext;
    SwrContext* swrContext;
    AVFrame* inputFrame;

    int bufferSize;
    uint8_t* samples;
    int samplesCursor;

    int swrBufferSize;
    uint8_t* swrBuffer;
    AVFrame* swrFrame;

    //音频时长
    double duration;
    double totalEncodeTimeMills;
    double totalSWRTimeMills;

    uint8_t** convertData;

    //通道数
    int channels;
    //采样率
    int sampleRate;
    //码率
    int bitRate;

    bool isWriteHeaderSuccess;

    char* outputAACPath;
    FILE* outputAACFile;

public:
    int init(int bitRate, int channels ,int sampleRate,int bitsPerSample,const char* aacFilePath,const char* codecName);
    void encode(byte* buffer,int size);
    void destroy();


    int allocAudioStream(const char *codecName);
    int allocAvFrame();

    void encodePacket();

    void writeAACPacketToFile(uint8_t *data, int size);

    void addADTSToPacket(uint8_t *buffer, int size);
};


#endif //NDK_AUDIORECODER_AUDIO_ENCODER_H
