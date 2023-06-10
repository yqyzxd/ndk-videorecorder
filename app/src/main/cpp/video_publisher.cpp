//
// Created by wind on 2023/5/25.
//

#include "video_publisher.h"


#define LOG_TAG "VideoPublisher"

VideoPublisher::VideoPublisher() {

}

VideoPublisher::~VideoPublisher() {

}

int
VideoPublisher::init(const char *outputUri, int videoFrameRate, int videoBitrate, int videoWidth,
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

    mHeaderHasWrite= false;

    int ret = avformat_alloc_output_context2(&mAVFormatContext, nullptr, "flv", outputUri);
    if (ret < 0) {
        LOGE("avformat_alloc_output_context2 error :", ret);
        return ret;
    }
    AVDictionary *opt = nullptr;
    AVOutputFormat *fmt = mAVFormatContext->oformat;
    fmt->video_codec = AV_CODEC_ID_H264;//使用h264编码器
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        ret = addStream(&mVideoStream, mAVFormatContext, &mVideoCodec, fmt->video_codec, nullptr);
        if (ret < 0) {
            LOGE("addStream error :", ret);
            return ret;
        }
        openVideo(mAVFormatContext, mVideoCodec, &mVideoStream, opt);
    }
    //fmt->audio_codec = AV_CODEC_ID_AAC;//使用fdk-aac编码器
    //注意：要使用libfdk_aac编码器必须使用avcodec_find_encoder_by_name，如果传入AV_CODEC_ID_AAC那么得到的是内置aac编码器，该内置编码器的sample_fmt为FLT而不是S16
    ret = addStream(&mAudioStream, mAVFormatContext, &mAudioCodec, AV_CODEC_ID_NONE, "libfdk_aac");
    if (ret < 0) {
        LOGE("addStream error :", ret);
        return ret;
    }
    openAudio(mAVFormatContext, mAudioCodec, &mAudioStream, opt);



    /**
     * Print detailed information about the input or output format, such as
     * duration, bitrate, streams, container, programs, metadata, side data,
     * codec and time base.
     */
    av_dump_format(mAVFormatContext,
                   0, outputUri, 1);

    if ((fmt->flags & AVFMT_NOFILE) == 0) {
        //todo 设置超时回调
        ret = avio_open(&mAVFormatContext->pb, outputUri, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGE("avio_open error :", ret);
            return
                    ret;
        }
    }

/* Write the stream header, if any. */
   /* ret = avformat_write_header(mAVFormatContext, &opt);
    if (ret < 0) {
        LOGE("avformat_write_header error :%s", av_err2str(ret));
        return
                ret;
    }
    mHeaderHasWrite = true;
    */
    return 0;

}


void VideoPublisher::setVideoPacketProvider(VideoPacketProvider provider, void *ctx) {
    this->mVideoProvider = provider;
    this->mVideoProviderCtx = ctx;
}

void VideoPublisher::setAudioPacketProvider(AudioPacketProvider provider, void *ctx) {
    this->mAudioProvider = provider;
    this->mAudioProviderCtx = ctx;
}

int VideoPublisher::addStream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec,
                              AVCodecID codecID, char *codecName) {
    AVCodecContext *c;

    if (AV_CODEC_ID_NONE == codecID) {
        *codec = avcodec_find_encoder_by_name(codecName);
    } else {
        *codec = avcodec_find_encoder(codecID);
    }
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
            c->codec_type = AVMEDIA_TYPE_VIDEO;
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
            c->gop_size = mVideoFrameRate;
            c->pix_fmt = AV_PIX_FMT_YUV420P;

            av_opt_set(c->priv_data, "preset", "ultrafast", 0);
            av_opt_set(c->priv_data, "tune", "zerolatency", 0);


            break;
        case AVMEDIA_TYPE_AUDIO:
            c->codec_id = codecID;
            c->codec_type = AVMEDIA_TYPE_AUDIO;
            c->sample_fmt = AV_SAMPLE_FMT_S16;
            c->bit_rate = mAudioBitrate;
            c->sample_rate = mAudioSampleRate;
            c->channel_layout = mAudioChannels == 1 ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
            c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
            ost->st->time_base = (AVRational) {1, c->sample_rate};
            c->time_base = ost->st->time_base;

            break;
        default:
            break;
    }
    /* Some formats want stream headers to be separate. */

    if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
        //LOGE("AVFMT_GLOBALHEADER");
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }


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
    LOGI("before mVideoProvider");
    int ret = mVideoProvider(&packet, mVideoProviderCtx);
    LOGI("after mVideoProvider: %d",ret);
    if (ret < 0) {
        return false;
    }
    AVCodecContext *c =ost.codecCtx;
    //栈上分配
    AVPacket pkt = {0};
    pkt.stream_index = ost.st->index;
    int naluType = packet->getNALUType();
    if (naluType == NALU_TYPE_SPS) {
        /**
         *  H.264/AVC extradata 语法
         *  bits   desc                                       data
         *   8     version                                  always 0x01
         *   8     avc profile                              sps[1]
         *   8     avc compatibility                        sps[2]
         *   8     avc level                                sps[3]
         *   6     reserved(NALULengthSizeMinusOne)         0xFC | 3 NAL长度字节数是4 减去1所以是3
         *   3     reserved(number of sps nalus usually 1)  0xE0 | 1
         *   16    sps size
         *   N     sps nalu data
         *   8     number of pps nalus usually 1
         *   16    pps size
         *   N     pps nalu data
         */

        //todo 封装codecContext的extradata
        X264Parser *parser = new X264Parser;
        std::vector<NALU *> *nalus = new std::vector<NALU *>();
        int ret = parser->parse(packet->buffer, packet->size, nalus);
        if (ret < 0) {
            //error
            return false;
        }
        if (nalus->size() != 2) {
            return false;
        }

        NALU *spsNalu = nalus->at(0);
        NALU *ppsNalu = nalus->at(1);


        int extraDataSize = 8 + 2 + spsNalu->size + 1 + ppsNalu->size;
       // uint8_t *extraData = static_cast<uint8_t *>(av_mallocz(extraDataSize));
        c->extradata = static_cast<uint8_t *>(av_mallocz(extraDataSize));
        c->extradata_size = extraDataSize;
        c->extradata[0] = 0x01;
        c->extradata[1] = spsNalu->body[1];
        c->extradata[2] = spsNalu->body[2];
        c->extradata[3] = spsNalu->body[3];

        c->extradata[4] = 0xFC | 3;
        c->extradata[5] = 0xE0 | 1;

        int spsLen = spsNalu->size;
        c->extradata[6] = (spsLen >> 8) & 0x00ff;
        c->extradata[7] = spsLen & 0x00ff;

        for (int i = 0; i < spsLen; ++i) {
            c->extradata[8 + i] = spsNalu->body[i];
        }

        c->extradata[8 + spsLen] = 0x01;

        int ppsLen = ppsNalu->size;
        c->extradata[8 + spsLen + 1] = (ppsLen >> 8) & 0x00ff;
        c->extradata[8 + spsLen + 2] = ppsLen & 0x00ff;

        for (int i = 0; i < ppsLen; ++i) {
            c->extradata[8 + spsLen + 3 + i] = ppsNalu->body[i];
        }


        /**
         * 注意：avcodec_open2之后如果没有执行avcodec_parameters_from_context将导致avformat_write_header  return -22  Invalid argument
         */
        ret = avformat_write_header(mAVFormatContext, nullptr);
        if (ret < 0) {
            //avformat_write_header error :Invalid argument
            LOGE("avformat_write_header error :%s", av_err2str(ret));
            return ret;
        }else{
            mHeaderHasWrite = true;
            LOGE("avformat_write_header success");
        }

    } else {
        int flag = 0;
        if (naluType == NALU_TYPE_IDR || naluType == NALU_TYPE_SEI) {
            flag = AV_PKT_FLAG_KEY;
        }


        int size= packet->size;
        pkt.data = packet->buffer;
        pkt.size = size;
        //计算pts/dts
        pkt.pts = packet->pts;
        pkt.dts = packet->dts;
        pkt.flags = flag;
        //pkt.duration=packet->duration;
        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(&pkt, ost.codecCtx->time_base, ost.st->time_base);

        if(pkt.data[0] == 0x00 && pkt.data[1] == 0x00 &&
           pkt.data[2] == 0x00 && pkt.data[3] == 0x01){
            size -= 4;
            pkt.data[0] = ((size ) >> 24) & 0x00ff;
            pkt.data[1] = ((size ) >> 16) & 0x00ff;
            pkt.data[2] = ((size ) >> 8) & 0x00ff;
            pkt.data[3] = ((size )) & 0x00ff;
        }

        c->frame_number++;
    }

    mCurVideoPacketPts = packet->timeMills;
    LOGE("packet->timeMills:%ld",mCurVideoPacketPts);
    logPacket(oc, &pkt);

    ret = av_interleaved_write_frame(oc, &pkt);
    LOGE("video av_interleaved_write_frame:%d",ret);
    av_packet_unref(&pkt);
    delete packet;
    if (ret < 0) {
        return false;
    }
    return true;
}

void VideoPublisher::logPacket(AVFormatContext *os, AVPacket *pkt) {
    AVRational *time_base = &os->streams[pkt->stream_index]->time_base;
    //pts:109208 pts_time:109.208 dts:NOPTS dts_time:NOPTS duration:0 duration_time:0 stream_index:0
    LOGI("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
         av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
         av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
         av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
         pkt->stream_index);
}

int VideoPublisher::encode() {
    int64_t curVideoTime = getVideoStreamTimeInSecs();
    int64_t curAudioTime = getAudioStreamTimeInSecs();
    int ret;
    LOGI("curVideoTime:%ld,curAudioTime:%ld",curVideoTime,curAudioTime);
    if (curAudioTime < curVideoTime) {
        ret = writeAudioFrame(mAVFormatContext, mAudioStream);
    } else {
        ret = writeVideoFrame(mAVFormatContext, mVideoStream);
    }
    return ret;
}

int VideoPublisher::stop() {
    if (mHeaderHasWrite) {
        /* Write the trailer, if any. The trailer must be written before you
    * close the CodecContexts open when you wrote the header; otherwise
    * av_write_trailer() may try to use memory that was freed on
    * av_codec_close(). */

        av_write_trailer(mAVFormatContext);

        LOGI("av_write_trailer and duration:%d", mAVFormatContext->duration);
        LOGI("av_write_trailer and stream duration:%d", mVideoStream.st->duration);

    }
    closeStream();

    if (!(mAVFormatContext->oformat->flags & AVFMT_NOFILE)) {
        /* Close the output file. */
        avio_closep(&mAVFormatContext->pb);
    }
    avformat_free_context(mAVFormatContext);
    return 0;
}

void VideoPublisher::closeStream() {
    avcodec_free_context(&mVideoStream.codecCtx);
    avcodec_free_context(&mAudioStream.codecCtx);
    //av_frame_free(&mVideoStream.avFrame);
    //av_frame_free(&mVideoStream.tmpFrame);
    //sws_freeContext(mVideoStream.swsCtx);
    //swr_free(&mVideoStream.swrCtx);

}

int VideoPublisher::openAudio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost,
                              AVDictionary *dict) {


    AVCodecContext *c = ost->codecCtx;
    AVDictionary *opt = nullptr;
    int ret = av_dict_copy(&opt, dict, 0);
    if (ret < 0) {
        LOGE("av_dict_copy error");
        return ret;
    }
    //todo 打开解码器
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        LOGE("avcodec_open2 error:%s", av_err2str(ret));
        return ret;
    }
    /**
     * AudioSpecificConfig的内容，完全可以通过ADTS的7字节头生成，具体来说，AudioSpecificConfig需要3部分数据：profile(LC,Main,HE)，sample_rate, channel，这3个数据在ADTS头里都可以找到
     * 有了这3个数据以后，可以进行合并生成2个字节，就是所谓的AudioSpecificConfig
     * ```
     * var profile:int = ((payload[2]&0xc0)>>6)+1;
     * var sample_rate:int = (payload[2]&0x3c)>>2;
     * var channel:int = ((payload[2]&0x1)<<2)|((payload[3]&0xc0)>>6);
     *
     * var config1:int = (profile<<3)|((sample_rate&0xe)>>1);
     * var config2:int = ((sample_rate&0x1)<<7)|(channel<<3);
     * var aacSeqHeader:ByteArray = new ByteArray();
     * aacSeqHeader.writeByte(config1);
     * aacSeqHeader.writeByte(config2);
     *
     * ```
     * profile取值
     *
     *  Object Type ID           	        Aduio Object Type	备注
     *      1	AAC Main
     *      2	AAC LC	最常用
     *      3	AAC LTR
     *      4	SBR
     *      5	AAC scalable
     */

    //AudioSpecificConfig
    c->extradata_size = 2;
    c->extradata = static_cast<uint8_t *>(av_mallocz(2));

    unsigned int profile = 2;
    char dsi[2];
    dsi[0] = (profile << 3) | (4 >> 1);
    dsi[1] = ((4 & 1) << 7) | (c->channels << 3);
    memcpy(c->extradata, dsi, 2);

    // c->extradata[0]=(profile<< 3)|((4&0xe)>>1); //44100 对应 4
    //c->extradata[1]= ((4&0x1)<<7)|(mAudioChannels<<3);

    /**
     * AAC音频格式有两种
     * ADIF(Audio Data Interchage Format)，音频数据交换格式：只有一个统一的头，必须得到所有数据后解码，适用于本地文件。
     * ADTS(Audio Data Transport Stream)，音视数据传输流：每一帧都有头信息，可在任意帧解码，适用于传输流。
     *
     * 1）将AAC编码器编码后的原始码流（ADTS头 + ES流）封装为MP4或者FLV或者MOV等格式时，需要先将ADTS头转换为MPEG-4 AudioSpecficConfig （将音频相关编解码参数提取出来），并将原始码流中的ADTS头去掉（只剩下ES流）。
     * 2）相反，从MP4或者FLV或者MOV等格式文件中解封装出AAC码流（只有ES流）时，需要在解析出的AAC码流前添加ADTS头（含音频相关编解码参数）。
     */
    //aac_adtstoasc作用：只是把带ADTS头的AAC流封装进MOV/MP4等格式时，创建MPEG-4 AudioSpecificConfig（asc），并去掉ADTS header
    mAudioBSFC = av_bitstream_filter_init("aac_adtstoasc");


    /* copy the stream parameters to the muxer */
    ret=avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret<0){
        LOGE("avcodec_parameters_from_context error");
    }
    return 0;
}

int64_t VideoPublisher::getVideoStreamTimeInSecs() {
    //LOGI("mCurVideoPacketPts:%ld",mCurVideoPacketPts);
    //return mCurVideoPacketPts / 1000.0f;
    return mCurVideoPacketPts;
}

int64_t VideoPublisher::getAudioStreamTimeInSecs() {
   // LOGI("mCurAudioPacketPts:%ld",mCurAudioPacketPts);
    //return mCurAudioPacketPts / 1000.0f;
    return mCurAudioPacketPts;
}

int VideoPublisher::writeAudioFrame(AVFormatContext *oc, OutputStream ost) {
    LOGI("enter writeAudioFrame");
    AudioPacket *packet;
    int ret = mAudioProvider(&packet, mAudioProviderCtx);
    if (ret < 0) {
        return ret;
    }
    LOGI("get AudioPacket from mAudioProvider");
    AVPacket pkt = {0};
    mCurAudioPacketPts = packet->position;
    LOGI("mCurAudioPacketPts:%d",mCurAudioPacketPts);
    pkt.data = packet->data;
    pkt.size = packet->size;
    pkt.dts = mCurAudioPacketPts;
    pkt.pts = mCurAudioPacketPts;
    pkt.duration = 1024;
    pkt.stream_index = ost.st->index;

    AVPacket dstPkt = {0};
    ret = av_bitstream_filter_filter(mAudioBSFC, ost.codecCtx, nullptr, &dstPkt.data, &dstPkt.size,
                                     pkt.data, pkt.size, pkt.flags & AV_PKT_FLAG_KEY);
    if (ret >= 0) {
        dstPkt.pts = pkt.pts;
        dstPkt.dts = pkt.dts;
        dstPkt.duration = pkt.duration;
        dstPkt.stream_index = pkt.stream_index;

        ret = av_interleaved_write_frame(oc, &pkt);
        LOGE("audio av_interleaved_write_frame:%d",ret);
        if (ret < 0) {
            LOGE("audio av_interleaved_write_frame error");
        }
        av_packet_unref(&pkt);
    }

    return 0;
}

