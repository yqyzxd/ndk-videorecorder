//
// Created by 史浩 on 2023/5/11.
//

#ifndef NDK_CAMERARECORDER_X264_ENCODER_H
#define NDK_CAMERARECORDER_X264_ENCODER_H


#include "../video_frame.h"
#include <stdio.h>
#include "../../utils/log.h"
extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
};
class X264Encoder {


public:
    X264Encoder();
    ~X264Encoder();

    int init(FILE* x264File,int width,int height,int bitRate,int frameRate);
    int encode(VideoFrame* frame);
    int dealloc();

private:
    FILE* h264File;
    AVCodec* mAVCodec= nullptr;
    AVCodecContext* mAVCodecContext= nullptr;
    AVFrame* mAVFrame= nullptr;
    AVFrame* mYUY2Frame= nullptr;

   AVPacket* mAvPacket;

};


#endif //NDK_CAMERARECORDER_X264_ENCODER_H
