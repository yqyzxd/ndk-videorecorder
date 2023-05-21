//
// Created by 史浩 on 2023/5/11.
//

#include "x264_encoder.h"
#include "libyuv/convert.h"
#include "color_conversion/color_conversion.h"

#define LOG_TAG "X264Encoder"

X264Encoder::X264Encoder() {

}

X264Encoder::~X264Encoder() {

}

int X264Encoder::init(FILE *x264File, int width, int height, int bitRate, int frameRate) {
    this->h264File = x264File;
    /////初始化AvCodecContext/////////////
    //mAVCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    mAVCodec = avcodec_find_encoder_by_name("libx264");
    if (mAVCodec == nullptr) {
        LOGE("avcodec_find_encoder error");
        return -1;
    }
    mAVCodecContext = avcodec_alloc_context3(mAVCodec);
    if (mAVCodecContext == nullptr) {
        LOGE("avcodec_alloc_context3 error");
        return -1;
    }
    mAVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    mAVCodecContext->width = width;
    mAVCodecContext->height = height;
    mAVCodecContext->time_base= {1,frameRate};
    mAVCodecContext->framerate={frameRate,1};
    mAVCodecContext->gop_size = frameRate;
    mAVCodecContext->max_b_frames = 0;
    //使用固定刻度
    mAVCodecContext->flags |= AV_CODEC_FLAG_QSCALE;
    mAVCodecContext->i_quant_factor = 0.8;
    mAVCodecContext->qmin = 10;
    mAVCodecContext->qmin = 30;
    mAVCodecContext->bit_rate = bitRate;
    //minimum bitrate
    mAVCodecContext->rc_min_rate = bitRate - 30 * 1000;
    mAVCodecContext->rc_max_rate = bitRate + 30 * 1000;
    //解码器位流缓冲区大小
    mAVCodecContext->rc_buffer_size = bitRate * 2;

    //av_opt_set(mAVCodecContext->priv_data, "preset", "slow", 0);
    //x264独有的参数
    av_opt_set(mAVCodecContext->priv_data, "preset", "ultrafast", 0);
    //调优 零延迟
    av_opt_set(mAVCodecContext->priv_data, "tune", "zerolatency", 0);
    av_opt_set(mAVCodecContext->priv_data, "profile", "main", 0);

    int ret = avcodec_open2(mAVCodecContext, mAVCodec, nullptr);
    if (ret < 0) {
        LOGE("avcodec_open2 error");
        return -1;
    }

    mAvPacket= av_packet_alloc();
    /////初始化YUV420P AvFrame/////////////
    mAVFrame = av_frame_alloc();
    mAVFrame->format=mAVCodecContext->pix_fmt;
    mAVFrame->width=mAVCodecContext->width;
    mAVFrame->height=mAVCodecContext->height;
    //初始化AvFrame的data和linesize
    av_frame_get_buffer(mAVFrame,0);


    mYUY2Frame = av_frame_alloc();
    mYUY2Frame->format=AV_PIX_FMT_YUYV422;
    mYUY2Frame->width=mAVCodecContext->width;
    mYUY2Frame->height=mAVCodecContext->height;
    av_frame_get_buffer(mYUY2Frame,0);

    /*int bufferSizeInYUV420 = av_image_get_buffer_size(mAVCodecContext->pix_fmt,
                                                      mAVCodecContext->width,
                                                      mAVCodecContext->height, 0);
    if (bufferSizeInYUV420 < 0) {
        LOGE("av_image_get_buffer_size error");
    }
    yuv420BufferSrc = (uint8_t *) av_malloc(bufferSizeInYUV420);
    ret=av_image_fill_arrays(mAVFrame->data, mAVFrame->linesize, yuv420BufferSrc,
                         mAVCodecContext->pix_fmt, mAVCodecContext->width, mAVCodecContext->height,
                         0);
    if (ret<0){
        LOGE("av_image_fill_arrays error");
    }

    /////YUY2Frame/////
    mYUY2Frame = av_frame_alloc();
    int bufferSizeInYUY2 = av_image_get_buffer_size(AV_PIX_FMT_YUYV422,
                                                    mAVCodecContext->width, mAVCodecContext->height,
                                                    0);
    if (bufferSizeInYUY2 < 0) {
        LOGE("av_image_get_buffer_size error");
    }
    yuy2BufferSrc = (uint8_t *) av_malloc(bufferSizeInYUY2);
    ret=av_image_fill_arrays(mYUY2Frame->data, mYUY2Frame->linesize, yuy2BufferSrc, AV_PIX_FMT_YUYV422,
                         mAVCodecContext->width, mAVCodecContext->height, 0);

    if (ret<0){
        LOGE("av_image_fill_arrays error");
    }*/
    return 0;
}

int X264Encoder::encode(VideoFrame *frame) {
    LOGI("enter encode");
    if (frame!= nullptr) {
        /* make sure the frame data is writable */
        int ret = av_frame_make_writable(mYUY2Frame);
        if (ret < 0){
            LOGI("av_frame_make_writable error");
            return ret;
        }
        ret = av_frame_make_writable(mAVFrame);
        if (ret < 0){
            LOGI("av_frame_make_writable error");
            return ret;
        }
        //将数据拷贝到avFrame的buffer
        memcpy(mYUY2Frame->data[0], frame->buffer, frame->size);
       /* libyuv::RGBAToI420(mYUY2Frame->data[0], mYUY2Frame->linesize[0],   mAVFrame->data[0], mAVFrame->linesize[0],
                                mAVFrame->data[1], mAVFrame->linesize[1],
                                mAVFrame->data[2], mAVFrame->linesize[2],
                                mAVCodecContext->width, mAVCodecContext->height);*/
        //todo 将yuy2数据转换成yuv420P格式
        //YUV格式在存储上存在两类布局：
        //Packed: 打包存储，放在一个数组中
        //Planar：平面模式，YUV三个分量分开存储在三个数组中
        //YUY2是packed方式
        //YUV420存储格式有YUV420P和YUV420SP，都是采用planar平面格式存储
        LOGI("before YUY2ToI420");
        libyuv::YUY2ToI420(mYUY2Frame->data[0], mYUY2Frame->linesize[0],
                           mAVFrame->data[0], mAVFrame->linesize[0],
                           mAVFrame->data[1], mAVFrame->linesize[1],
                           mAVFrame->data[2], mAVFrame->linesize[2],
                           mAVCodecContext->width, mAVCodecContext->height);

        //yuy2_to_yuv420p(mYUY2Frame->data, mYUY2Frame->linesize, mAVCodecContext->width, mAVCodecContext->height, mAVFrame->data, mAVFrame->linesize);

        LOGI("after YUY2ToI420");
        int presentationTimeInMills = frame->timeInMills;

        int pts = presentationTimeInMills;
        mAVFrame->pts = pts;
        LOGI("mAVFrame pts:%d", pts);
    }
    int ret = avcodec_send_frame(mAVCodecContext, mAVFrame);
    while (ret>=0){
        ret = avcodec_receive_packet(mAVCodecContext, mAvPacket);
        LOGE("avcodec_send_frame return :%s", av_err2str(ret));
        //if (ret== AVERROR(EAGAIN) || ret ==AVERROR_EOF){}
        if (ret>=0){
            LOGE("Write packet %3d (size=%5d)\n", mAvPacket->pts, mAvPacket->size);
            fwrite(mAvPacket->data,1,mAvPacket->size,h264File);
        }
        av_packet_unref(mAvPacket);

    }
    return 0;

}

int X264Encoder::dealloc() {
    av_frame_free(&mAVFrame);
    mAVFrame= nullptr;
    av_frame_free(&mYUY2Frame);

    /* flush the encoder */
    encode(nullptr);

    /* add sequence end code to have a real MPEG file */
    uint8_t endCode[] = { 0, 0, 1, 0xb7 };
    if (mAVCodec->id == AV_CODEC_ID_MPEG1VIDEO || mAVCodec->id == AV_CODEC_ID_MPEG2VIDEO)
        fwrite(endCode, 1, sizeof(endCode), h264File);
    fclose(h264File);
    LOGE("fclose");
    avcodec_free_context(&mAVCodecContext);
    av_packet_free(&mAvPacket);

    return 0;
}