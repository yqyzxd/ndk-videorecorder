//
// Created by wind on 2023/5/25.
//

#ifndef NDK_VIDEORECORDER_VIDEO_PUBLISHER_H
#define NDK_VIDEORECORDER_VIDEO_PUBLISHER_H


#include "encoder/video_frame.h"
#include "utils/log.h"


extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
};

typedef int (*VideoPacketProvider)(VideoPacket** pkt,void* ctx);

typedef struct OutputStream{
    AVStream* st;
    AVCodecContext *codecCtx;
    /* pts of the next frame that will be generated */
    int64_t nextPts;
    int samplesCount;
    AVFrame* avFrame;
    AVFrame* tmpFrame;

    float t,tincr,tinc2;
    SwsContext* swsCtx;
    SwrContext* swrCtx;
}OutputStream;

class VideoPublisher {
public:
    VideoPublisher();
    ~VideoPublisher();


    virtual int init(char* outputUri,
                     int videoFrameRate,int videoBitrate,int videoWidth,int videoHeight,
                     int audioBitrate,int audioSampleRate,int audioChannels);

    virtual void setVideoPacketProvider(VideoPacketProvider provider,void* ctx);


protected:
    const char *mOutputUri;

    int mVideoFrameRate;
    int mVideoBitrate;
    int mVideoWidth;
    int mVideoHeight;

    int mAudioBitrate;
    int mAudioSampleRate;
    int mAudioChannels;

    VideoPacketProvider mVideoProvider;
    void *mVideoProviderCtx;


    AVOutputFormat* mAVOutputFormat;
    AVFormatContext* mAVFormatContext;

    OutputStream mVideoStream;
    AVCodec* mVideoCodec;

    int addStream(OutputStream *outputStream, AVFormatContext* oc,AVCodec **codec, AVCodecID id);

    int openVideo(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *dict);

    AVFrame *allocPicture(AVPixelFormat format, int width, int height);

    bool writeVideoFrame(AVFormatContext *oc, OutputStream ost);

    void logPacket(AVFormatContext *os, AVPacket *pkt);
};


#endif //NDK_VIDEORECORDER_VIDEO_PUBLISHER_H
