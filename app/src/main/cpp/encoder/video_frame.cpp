//
// Created by 史浩 on 2023/5/10.
//

#include "video_frame.h"
VideoFrame::VideoFrame() {
    buffer= nullptr;
    size=0;
    timeInMills=0;
    pts=PTS_NONE_FLAG;
    dts=DTS_NONE_FLAG;
}

VideoFrame::~VideoFrame() {
    if (buffer){
        delete buffer;
        buffer= nullptr;
    }

}