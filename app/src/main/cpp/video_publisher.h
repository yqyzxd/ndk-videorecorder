//
// Created by wind on 2023/5/25.
//

#ifndef NDK_VIDEORECORDER_VIDEO_PUBLISHER_H
#define NDK_VIDEORECORDER_VIDEO_PUBLISHER_H


#include "encoder/video_frame.h"
#include "utils/log.h"
#include "audio_frame.h"


extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavutil/timestamp.h"
};

typedef int (*VideoPacketProvider)(VideoPacket** pkt,void* ctx);
typedef int (*AudioPacketProvider)(AudioPacket** pkt,void* ctx);

typedef struct OutputStream{
    AVStream* st;
    AVCodecContext *codecCtx;
    /* pts of the next frame that will be generated */
    int64_t nextPts;
    int samplesCount;
    AVFrame* avFrame;
    AVFrame* tmpFrame;

    float t,tincr,tincr2;
    SwsContext* swsCtx;
    SwrContext* swrCtx;
}OutputStream;

class VideoPublisher {
public:
    VideoPublisher();
    ~VideoPublisher();


    virtual int init(const char* outputUri,
                     int videoFrameRate,int videoBitrate,int videoWidth,int videoHeight,
                     int audioBitrate,int audioSampleRate,int audioChannels);

     void setVideoPacketProvider(VideoPacketProvider provider,void* ctx);
     void setAudioPacketProvider(AudioPacketProvider provider,void* ctx);
    int encode();
    int stop();

protected:
    const char *mOutputUri;

    int mVideoFrameRate;
    int mVideoBitrate;
    int mVideoWidth;
    int mVideoHeight;

    int mAudioBitrate;
    int mAudioSampleRate;
    int mAudioChannels;

    //是否已经写入了文件头
    bool mHeaderHasWrite= false;

    VideoPacketProvider mVideoProvider;
    void *mVideoProviderCtx;

    AudioPacketProvider mAudioProvider;
    void *mAudioProviderCtx;




    double mCurAudioPacketPts;
    double mCurVideoPacketPts;


    AVOutputFormat* mAVOutputFormat;
    AVFormatContext* mAVFormatContext;

    OutputStream mVideoStream;
    AVCodec* mVideoCodec;

    OutputStream mAudioStream;
    AVCodec* mAudioCodec;
    AVBitStreamFilterContext *audioBSFC;

    int addStream(OutputStream *outputStream, AVFormatContext* oc,AVCodec **codec, AVCodecID id);

    int openVideo(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *dict);

    AVFrame *allocPicture(AVPixelFormat format, int width, int height);

    bool writeVideoFrame(AVFormatContext *oc, OutputStream ost);

    void logPacket(AVFormatContext *os, AVPacket *pkt);



    void closeStream();

    int openAudio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt);


    double getVideoStreamTimeInSecs();

    double getAudioStreamTimeInSecs();

    int writeAudioFrame(AVFormatContext *oc, OutputStream ost);
};


#endif //NDK_VIDEORECORDER_VIDEO_PUBLISHER_H
