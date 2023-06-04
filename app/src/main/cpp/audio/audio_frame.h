//
// Created by wind on 2023/5/29.
//

#ifndef NDK_VIDEORECORDER_AUDIO_FRAME_H
#define NDK_VIDEORECORDER_AUDIO_FRAME_H
#include "../utils/types.h"
typedef struct AudioPacket {
    byte *data= nullptr;
    short* buffer= nullptr;
    int size=0;
    float position=0;

    ~AudioPacket() {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }

} AudioPacket;

#endif //NDK_VIDEORECORDER_AUDIO_FRAME_H
