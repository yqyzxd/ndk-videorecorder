//
// Created by 史浩 on 2023/5/10.
//

#ifndef NDK_CAMERARECORDER_VIDEO_FRAME_H
#define NDK_CAMERARECORDER_VIDEO_FRAME_H

#define PTS_NONE_FLAG   -1
#define DTS_NONE_FLAG   -1

#include <cstdint>
#include "../utils/types.h"


class VideoFrame {
public:
    VideoFrame();
    ~VideoFrame();
public:
  byte* buffer;
  int size;
  int timeInMills;
  int64_t pts;
  int64_t dts;
};


#endif //NDK_CAMERARECORDER_VIDEO_FRAME_H
