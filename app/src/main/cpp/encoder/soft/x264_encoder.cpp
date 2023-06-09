//
// Created by 史浩 on 2023/5/11.
//

#include "x264_encoder.h"
#include "libyuv/convert.h"


X264Encoder::X264Encoder() {
    mBitrateMode = "cbr";
}

X264Encoder::~X264Encoder() {

}

int X264Encoder::init(int width, int height, int bitRate, int frameRate) {
    mVideoPacketPool = VideoPacketPool::getInstance();
    mX264Parser = new X264Parser();
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
    mAVCodecContext->time_base = {1, frameRate};
    mAVCodecContext->framerate = {frameRate, 1};
    mAVCodecContext->gop_size = frameRate;
    mAVCodecContext->max_b_frames = 0;//禁止插入B帧
    // todo 增加strategy判断使用cbr模式（恒定比特率）还是vbr模式（可变比特率）
    if (strcmp(mBitrateMode, "cbr") == 0) {
        //cbr参数设置
        mAVCodecContext->bit_rate = bitRate;
        mAVCodecContext->rc_min_rate = bitRate;
        mAVCodecContext->rc_max_rate = bitRate;
        mAVCodecContext->bit_rate_tolerance = bitRate;
        mAVCodecContext->rc_buffer_size = bitRate;
        mAVCodecContext->rc_initial_buffer_occupancy = mAVCodecContext->rc_buffer_size * 3 / 4;

        //mAVCodecContext->rc_buffer_aggressivity= (float)1.0;
        //mAVCodecContext->rc_initial_cplx= 0.5;
    } else {
        //VBR参数设置
        mAVCodecContext->flags |= AV_CODEC_FLAG_QSCALE;
        mAVCodecContext->i_quant_factor = 0.8;
        mAVCodecContext->qmin = 10;
        mAVCodecContext->qmin = 30;
        mAVCodecContext->bit_rate = bitRate;
        mAVCodecContext->rc_min_rate = bitRate / 2;
        mAVCodecContext->rc_max_rate = bitRate * 2;
    }
    //不设置该值，则每帧前面都会存放metadata 比如视频的sps/pps
    //mAVCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


    //minimum bitrate
    //mAVCodecContext->rc_min_rate = bitRate - 30 * 1000;
    //mAVCodecContext->rc_max_rate = bitRate + 30 * 1000;
    //解码器位流缓冲区大小
    //mAVCodecContext->rc_buffer_size = bitRate * 2;

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

    mAvPacket = av_packet_alloc();
    /////初始化YUV420P AvFrame/////////////
    mAVFrame = av_frame_alloc();
    mAVFrame->format = mAVCodecContext->pix_fmt;
    mAVFrame->width = mAVCodecContext->width;
    mAVFrame->height = mAVCodecContext->height;
    //初始化AvFrame的data和linesize
    av_frame_get_buffer(mAVFrame, 0);


    mYUY2Frame = av_frame_alloc();
    mYUY2Frame->format = AV_PIX_FMT_YUYV422;
    mYUY2Frame->width = mAVCodecContext->width;
    mYUY2Frame->height = mAVCodecContext->height;
    av_frame_get_buffer(mYUY2Frame, 0);


    return 0;
}

int X264Encoder::encode(VideoFrame *frame) {

    if (frame != nullptr) {
        /* make sure the frame data is writable */
        int ret = av_frame_make_writable(mYUY2Frame);
        if (ret < 0) {
            LOGI("av_frame_make_writable error");
            return ret;
        }
        ret = av_frame_make_writable(mAVFrame);
        if (ret < 0) {
            LOGI("av_frame_make_writable error");
            return ret;
        }
        //将数据拷贝到avFrame的buffer
        memcpy(mYUY2Frame->data[0], frame->buffer, frame->size);

        //将yuy2数据转换成yuv420P格式
        //YUV格式在存储上存在两类布局：
        //Packed: 打包存储，放在一个数组中
        //Planar：平面模式，YUV三个分量分开存储在三个数组中
        //YUY2是packed方式
        //YUV420存储格式有YUV420P和YUV420SP，都是采用planar平面格式存储

        libyuv::YUY2ToI420(mYUY2Frame->data[0], mYUY2Frame->linesize[0],
                           mAVFrame->data[0], mAVFrame->linesize[0],
                           mAVFrame->data[1], mAVFrame->linesize[1],
                           mAVFrame->data[2], mAVFrame->linesize[2],
                           mAVCodecContext->width, mAVCodecContext->height);

        //int64_t presentationTimeInMills = frame->timeInMills;

        //int64_t pts = presentationTimeInMills;
        mAVFrame->pts = mNextPts++;
        //LOGI("mAVFrame pts:%d", pts);
    }
    int ret = avcodec_send_frame(mAVCodecContext, mAVFrame);
    while (ret >= 0) {
        ret = avcodec_receive_packet(mAVCodecContext, mAvPacket);
        if (ret >= 0) {
            parsePacketAndEnqueue();
        }
        av_packet_unref(mAvPacket);

    }
    return 0;

}

int X264Encoder::dealloc() {
    av_frame_free(&mAVFrame);
    mAVFrame = nullptr;
    av_frame_free(&mYUY2Frame);

    /* flush the encoder */
    encode(nullptr);



    // LOGE("fclose");
    avcodec_free_context(&mAVCodecContext);
    av_packet_free(&mAvPacket);

    return 0;
}

VideoPacket *
X264Encoder::buildVideoPacket(byte *buffer, int size, int64_t timeMillis, int64_t pts, int64_t dts,
                              int64_t duration) {
    VideoPacket *pkt = new VideoPacket();
    pkt->timeMills = timeMillis;
    pkt->buffer = buffer;
    pkt->size = size;
    pkt->pts = pts;
    pkt->dts = dts;
    pkt->duration = duration;
    //LOGI("x264 AVPacket timeMillis:%d,pts:%d,dts:%d,duration:%d",timeMillis,pts,dts,duration);
    return pkt;
}

void X264Encoder::parsePacketAndEnqueue() {

    //fwrite(mAvPacket->data,1,mAvPacket->size,h264File);//生成h264文件


    //解析mAvPacket->data h264数据
    std::vector<NALU *> *nalus = new std::vector<NALU *>();
    int ret = mX264Parser->parse(reinterpret_cast<uint8_t *>(mAvPacket->data), mAvPacket->size, nalus);

    if (ret < 0) {
        LOGE("x264 parse error");
    } else {

        byte *frameBuffer;
        const char headCode[] = "\x00\x00\x00\x01";
        int frameBufferSize = 0;
        int headCodeLen = 4;

        if (nalus->at(0)->type == NALU_TYPE_SPS) {
            if (!mAlreadyWriteSPS) {
                mAlreadyWriteSPS = true;
                NALU *spsNALU = nalus->at(0);
                uint8 *spsData = spsNALU->body;
                int spsLen = spsNALU->size;

                NALU *ppsNALU = nalus->at(1);
                uint8 *ppsData = ppsNALU->body;
                int ppsLen = ppsNALU->size;

                int bufferSize = headCodeLen + spsLen + headCodeLen + ppsLen;
                byte *buffer = new byte[bufferSize];
                memcpy(buffer, headCode, headCodeLen);
                memcpy(buffer + headCodeLen, spsData, spsLen);
                memcpy(buffer + headCodeLen + spsLen, headCode, headCodeLen);
                memcpy(buffer + headCodeLen + spsLen + headCodeLen, ppsData, ppsLen);

                // insert to queue
                mVideoPacketPool->enqueueVideoPacket(
                        buildVideoPacket(buffer, bufferSize, mAvPacket->pts, mAvPacket->pts,
                                         mAvPacket->dts, mAvPacket->duration));

            }
        }
        //计算去除sps和pps的总长度

        std::vector<NALU *>::iterator iter;
        for (iter = nalus->begin(); iter != nalus->end(); iter++) {
            NALU *nalu = *iter;
            if (nalu->type != NALU_TYPE_SPS && nalu->type != NALU_TYPE_PPS) {
                frameBufferSize += headCodeLen;
                frameBufferSize += nalu->size;
            }
        }
        // h264 有两种码流格式 annexb和 avcc
        //现在输出的是annexb格式 ，所以封装到flv或mp4需要转化为avcc格式
        //avcc格式 准名称是MPEG-4 Byte Stream Format，适合文件存储 又叫AVC1格式，FLV、MP4、MKV文件用的都是这种格式
        //ExtraData | Length NALU | Length NALU | …  length长度是4个自己，所以这边把frameLen保存在前四个字节中。ExtraData对应AVCodecContext的extradata
        frameBuffer = new byte[frameBufferSize];
        int frameBufferCursor = 0;
        for (iter = nalus->begin(); iter != nalus->end(); iter++) {
            NALU *nalu = *iter;
            if (nalu->type != NALU_TYPE_SPS && nalu->type != NALU_TYPE_PPS) {
                uint8_t *naluBody = nalu->body;
                int naluSize = nalu->size;

                //填充frameBuffer：  格式为Length NALU
                memcpy(frameBuffer + frameBufferCursor, headCode,
                       headCodeLen);//长度占4个字节
                frameBufferCursor += headCodeLen;
                memcpy(frameBuffer + frameBufferCursor, naluBody, naluSize);
                frameBufferCursor += naluSize;

                //将前4个字节修改成nalu的长度
                frameBuffer[frameBufferCursor - naluSize - headCodeLen] =
                        (naluSize >> 24) & 0x00ff;
                frameBuffer[frameBufferCursor - naluSize - headCodeLen + 1] =
                        (naluSize >> 16) & 0x00ff;
                frameBuffer[frameBufferCursor - naluSize - headCodeLen + 2] =
                        (naluSize >> 8) & 0x00ff;
                frameBuffer[frameBufferCursor - naluSize - headCodeLen + 3] =
                        (naluSize) & 0x00ff;


            }
            delete nalu;
        }
        delete nalus;
        mVideoPacketPool->enqueueVideoPacket(
                buildVideoPacket(frameBuffer, frameBufferSize, mAvPacket->pts, mAvPacket->pts,
                                 mAvPacket->dts, mAvPacket->duration));

    }
    av_packet_unref(mAvPacket);

}


