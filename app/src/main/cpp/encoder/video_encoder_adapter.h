//
// Created by 史浩 on 2023/5/10.
//

#ifndef NDK_CAMERARECORDER_VIDEO_ENCODER_ADAPTER_H
#define NDK_CAMERARECORDER_VIDEO_ENCODER_ADAPTER_H


#include "../libgles/gles/gl_utils.h"
#include <stdio.h>
#include <EGL/egl.h>

class VideoEncoderAdapter {
public:
    VideoEncoderAdapter();
    virtual ~VideoEncoderAdapter(){};

    virtual int init(const char* output,int width,int height,int bitRate,int frameRate);
    virtual void prepare(EGLContext sharedContext)=0;
    virtual void encode(int textureId)=0;
    virtual void stop()=0;
    virtual void dealloc()=0;

protected:
    FILE* h264File;
    int width;
    int height;
    int bitRate;
    int frameRate;

    //编码开始时间
    int64_t mStartTimeInMills=-1;
    //已经编码的帧数，用于帧率控制
    int mEncodedFrameCount;
};


#endif //NDK_CAMERARECORDER_VIDEO_ENCODER_ADAPTER_H
