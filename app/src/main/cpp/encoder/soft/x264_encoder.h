//
// Created by 史浩 on 2023/5/11.
//

#ifndef NDK_CAMERARECORDER_X264_ENCODER_H
#define NDK_CAMERARECORDER_X264_ENCODER_H


#include "../video_frame.h"
#include <stdio.h>
#include "../../utils/log.h"
#include "x264_parser.h"
#include "../../video_packet_pool.h"

extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavutil/timestamp.h"
};
class X264Encoder {


public:
    X264Encoder();
    ~X264Encoder();

    int init(int width,int height,int bitRate,int frameRate);
    int encode(VideoFrame* frame);
    int dealloc();

private:
    AVCodec* mAVCodec= nullptr;
    AVCodecContext* mAVCodecContext= nullptr;
    AVFrame* mAVFrame= nullptr;
    AVFrame* mYUY2Frame= nullptr;
    AVPacket* mAvPacket;
    //比特率模式 值为 vbr/cbr
    const char* mBitrateMode;
    VideoPacketPool* mVideoPacketPool=0;

   bool mPrinted;
    X264Parser* mX264Parser;
    bool mAlreadyWriteSPS =false;

    VideoPacket *buildVideoPacket(byte *buffer, int size, int64_t timeMillis,int64_t pts, int64_t dts);
};


#endif //NDK_CAMERARECORDER_X264_ENCODER_H
