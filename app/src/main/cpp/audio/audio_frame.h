//
// Created by wind on 2023/5/29.
//

#ifndef NDK_VIDEORECORDER_AUDIO_FRAME_H
#define NDK_VIDEORECORDER_AUDIO_FRAME_H
#include "../utils/types.h"

typedef struct AudioFrame {

    short* buffer= nullptr;
    int size=0;
    int64_t position=0;
    int64_t duration=0;
    int64_t pts=0;
    int64_t dts=0;
    ~AudioFrame() {
        if (buffer != nullptr) {
            delete[] buffer;
            buffer = nullptr;
        }
    }

} AudioFrame;


typedef struct AudioPacket {
    byte* data= nullptr;
    int size=0;
    int64_t position=0;
    int64_t duration=0;
    int64_t pts=0;
    int64_t dts=0;
    ~AudioPacket() {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }

} AudioPacket;

#endif //NDK_VIDEORECORDER_AUDIO_FRAME_H
