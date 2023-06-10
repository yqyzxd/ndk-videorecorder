//
// Created by 史浩 on 2023/5/10.
//

#ifndef NDK_CAMERARECORDER_VIDEO_FRAME_H
#define NDK_CAMERARECORDER_VIDEO_FRAME_H

#define PTS_NONE_FLAG   -1
#define DTS_NONE_FLAG   -1

#include <cstdint>
#include "../utils/types.h"
#include "soft/x264_parser.h"
#define LOG_TAG "VideoFrame"
#include "../utils/log.h"
class VideoFrame {
public:
    VideoFrame(){
        buffer= nullptr;
        size=0;
        timeInMills=0;
    };
    ~VideoFrame(){
        if (buffer){
            delete buffer;
            buffer= nullptr;
        }
    };
public:
    byte* buffer;
    int size;
    int64_t timeInMills;
};


class VideoPacket {
public:
    VideoPacket(){
        buffer= nullptr;
        size=0;
        timeMills=0;
        pts=PTS_NONE_FLAG;
        dts=DTS_NONE_FLAG;
    };
    ~VideoPacket(){
        if (buffer){
            delete buffer;
            buffer= nullptr;
        }
    };
    int getNALUType(){
        int nalu_type = NALU_TYPE_NON_IDR;
        if(NULL != buffer){
            nalu_type = (buffer[4] & 0x1F);
        }
        LOGI("nalu_type:%d",nalu_type);
        return nalu_type;
    }
public:
    byte* buffer;
    int size;
    int64_t timeMills;
    int duration;
    int64_t pts;
    int64_t dts;
};


#endif //NDK_CAMERARECORDER_VIDEO_FRAME_H
