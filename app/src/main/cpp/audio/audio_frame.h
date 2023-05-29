//
// Created by wind on 2023/5/29.
//

#ifndef NDK_VIDEORECORDER_AUDIO_FRAME_H
#define NDK_VIDEORECORDER_AUDIO_FRAME_H

typedef struct AudioPacket {
    short *buffer;
    int size;
    float position;

    ~AudioPacket() {
        if (buffer != nullptr) {
            delete[] buffer;
            buffer = nullptr;
        }
    }

} AudioPacket;

#endif //NDK_VIDEORECORDER_AUDIO_FRAME_H
