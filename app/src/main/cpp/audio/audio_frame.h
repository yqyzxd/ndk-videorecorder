//
// Created by wind on 2023/5/29.
//

#ifndef NDK_VIDEORECORDER_AUDIO_FRAME_H
#define NDK_VIDEORECORDER_AUDIO_FRAME_H

typedef struct AudioPacket {
    byte *data;
    int size;
    float position;

    ~AudioPacket() {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }

} AudioPacket;

#endif //NDK_VIDEORECORDER_AUDIO_FRAME_H
