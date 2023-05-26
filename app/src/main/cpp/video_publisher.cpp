//
// Created by wind on 2023/5/25.
//

#include "video_publisher.h"
#include "libavutil/opt.h"
#include "libavutil/timestamp.h"

#define LOG_TAG "VideoPublisher"

VideoPublisher::VideoPublisher() {

}

VideoPublisher::~VideoPublisher() {

}

int VideoPublisher::init(char *outputUri, int videoFrameRate, int videoBitrate, int videoWidth,
                         int videoHeight, int audioBitrate, int audioSampleRate,
                         int audioChannels) {
    mOutputUri = outputUri;
    mVideoFrameRate = videoFrameRate;
    mVideoBitrate = videoBitrate;
    mVideoWidth = videoWidth;
    mVideoHeight = videoHeight;

    mAudioBitrate = audioBitrate;
    mAudioSampleRate = audioSampleRate;
    mAudioChannels = audioChannels;


    int ret = avformat_alloc_output_context2(&mAVFormatContext, nullptr, "flv", outputUri);
    if (ret < 0) {
        LOGE("avformat_alloc_output_context2 error :", ret);
        return ret;
    }
    AVDictionary *opt = nullptr;
    AVOutputFormat *fmt = mAVFormatContext->oformat;
    fmt->video_codec = AV_CODEC_ID_H264;//使用h264编码器
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        ret = addStream(&mVideoStream, mAVFormatContext, &mVideoCodec, fmt->video_codec);
        if (ret < 0) {
            LOGE("addStream error :", ret);
            return ret;
        }
        openVideo(mAVFormatContext, mVideoCodec, &mVideoStream, opt);
    }
    /**
     * Print detailed information about the input or output format, such as
     * duration, bitrate, streams, container, programs, metadata, side data,
     * codec and time base.
     */
    av_dump_format(mAVFormatContext, 0, outputUri, 1);

    if ((fmt->flags & AVFMT_NOFILE) == 0) {
        ret = avio_open(&mAVFormatContext->pb, outputUri, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGE("avio_open error :", ret);
            return ret;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(mAVFormatContext, &opt);
    if (ret < 0) {
        LOGE("avformat_write_header error :%s", av_err2str(ret));
        return ret;
    }
    bool encodeVideo = true;
    while (encodeVideo) {
        encodeVideo = writeVideoFrame(mAVFormatContext, mVideoStream);
    }
}

void VideoPublisher::setVideoPacketProvider(VideoPacketProvider provider, void *ctx) {
    this->mVideoProvider = provider;
    this->mVideoProviderCtx = ctx;
}

int VideoPublisher::addStream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec,
                              AVCodecID codecID) {
    AVCodecContext *c;

    *codec = avcodec_find_encoder(codecID);
    if (!(*codec)) {
        LOGE("avcodec_find_encoder error");
        return -1;
    }
    ost->st = avformat_new_stream(oc, nullptr);
    if (!ost->st) {
        LOGE("avformat_new_stream error");
        return -2;
    }
    ost->st->id = oc->nb_streams - 1;

    c = avcodec_alloc_context3(*codec);
    if (!c) {
        LOGE("avcodec_alloc_context3 error");
        return -3;
    }
    ost->codecCtx = c;

    switch ((*codec)->type) {
        case AVMEDIA_TYPE_VIDEO:
            c->codec_id = codecID;
            c->bit_rate = mVideoBitrate;
            /* Resolution must be a multiple of two. */
            c->width = mVideoWidth;
            c->height = mVideoHeight;
            /* timebase: This is the fundamental unit of time (in seconds) in terms
             * of which frame timestamps are represented. For fixed-fps content,
             * timebase should be 1/framerate and timestamp increments should be
             * identical to 1.
             */
            ost->st->time_base = (AVRational) {1, mVideoFrameRate};
            c->time_base = ost->st->time_base;
            c->gop_size = 12;
            c->pix_fmt = AV_PIX_FMT_YUV420P;

            av_opt_set(c->priv_data, "preset", "ultrafast", 0);
            av_opt_set(c->priv_data, "tune", "zerolatency", 0);


            break;
    }
    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    return 0;
}

int VideoPublisher::openVideo(AVFormatContext *oc, AVCodec *codec, OutputStream *ost,
                              AVDictionary *dict) {
    AVCodecContext *c = ost->codecCtx;
    AVDictionary *opt = nullptr;
    int ret = av_dict_copy(&opt, dict, 0);
    if (ret < 0) {
        LOGE("av_dict_copy error");
        return ret;
    }
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        LOGE("avcodec_open2 error");
        return ret;
    }
    /* allocate and init a re-usable frame */
    ost->avFrame = allocPicture(c->pix_fmt, c->width, c->height);
    if (ost->avFrame == nullptr) {
        return -1;
    }
    ost->tmpFrame = nullptr;
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        ost->tmpFrame = allocPicture(AV_PIX_FMT_YUV420P, c->width, c->height);
        if (ost->tmpFrame == nullptr) {
            return -1;
        }
    }
    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        LOGE("avcodec_parameters_from_context error:%d", ret);
        return ret;
    }

    return 0;
}

AVFrame *VideoPublisher::allocPicture(AVPixelFormat pix_fmt, int width, int height) {
    AVFrame *picture = av_frame_alloc();
    if (picture == nullptr) {
        return nullptr;
    }
    picture->format = pix_fmt;
    picture->width = width;
    picture->height = height;
    /* allocate the buffers for the frame data */
    int ret = av_frame_get_buffer(picture, 0);
    if (ret < 0) {
        LOGE("av_frame_get_buffer error");
        return nullptr;
    }

    return picture;
}

bool VideoPublisher::writeVideoFrame(AVFormatContext *oc, OutputStream ost) {
    VideoPacket *packet;
    int ret = mVideoProvider(&packet, mVideoProviderCtx);
    if (ret < 0) {
        return false;
    }
    //栈上分配
    AVPacket pkt ={0};

    pkt.data=packet->buffer;
    pkt.size=packet->size;
    pkt.pts=packet->pts;
    pkt.dts=packet->dts;
    pkt.duration=packet->duration;
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(&pkt,ost.codecCtx->time_base,ost.st->time_base);
    pkt.stream_index=ost.st->index;


    logPacket(mAVFormatContext,&pkt);
    ret = av_interleaved_write_frame(mAVFormatContext, &pkt);
    av_packet_unref(&pkt);
    if (ret<0){
        return false;
    }
    return true;
}

void VideoPublisher::logPacket(AVFormatContext *os, AVPacket *pkt) {
    AVRational *time_base = &os->streams[pkt->stream_index]->time_base;

    LOGI("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
         av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
         av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
         av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
         pkt->stream_index);
}
