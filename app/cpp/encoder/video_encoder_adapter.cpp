//
// Created by 史浩 on 2023/5/10.
//

#include "video_encoder_adapter.h"
VideoEncoderAdapter::VideoEncoderAdapter() {

}
int VideoEncoderAdapter::init(const char *output, int width, int height, int bitRate, int frameRate) {

    h264File=fopen(output,"wb+");
    this->width=width;
    this->height=height;
    this->bitRate=bitRate;
    this->frameRate=frameRate;
    return 0;
}

