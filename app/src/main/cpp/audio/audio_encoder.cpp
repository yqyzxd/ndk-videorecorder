//
// Created by wind on 2023/4/24.
//

#include "audio_encoder.h"



#define LOG_TAG "AudioEncoder"
int AudioEncoder::init(int bitRate, int recordChannels, int sampleRate, int bitsPerSample,
                        const char *codecName) {
    mAudioNextPts=0;
    swrBuffer= nullptr;
    swrContext= nullptr;

    swrFrame= nullptr;
    inputFrame= nullptr;

    avCodecContext= nullptr;


    this->mBitRate=bitRate;
    this->mRecordChannels=recordChannels;
    //this->mTargetChannels=2;
    this->mSampleRate=sampleRate;


    mAudioFrameProvider= nullptr;
    mAudioFrameProviderCtx= nullptr;
    mAudioPacketCollector= nullptr;
    mAudioPacketCollectorCtx= nullptr;

    //创建AudioStream
   int  ret=allocAudioStream(codecName);
    if (ret < 0) {
        LOGI("allocAudioStream error");
        return -1;
    }

    //创建AvFrame
    allocAvFrame();

    return 0;
}

void AudioEncoder::setAudioFrameProvider(AudioFrameProvider provider, void *ctx) {
    this->mAudioFrameProvider=provider;
    this->mAudioFrameProviderCtx=ctx;
}
int AudioEncoder::encode() {
    int64_t pts=0;
    int actualSampleSizeInShort=mAudioFrameProvider((short *)(inputFrame->data[0]), nbSamples, mRecordChannels, &pts, mAudioFrameProviderCtx);
    if(actualSampleSizeInShort<=0){
        LOGI("provide audio frame error");
        return -1;
    }
    int ret=encodePacket();
    return ret;
}


int AudioEncoder::encodePacket() {

    AVRational timeBase={1,mSampleRate};

    AVPacket pkt;
    av_init_packet(&pkt);
    //av_packet_alloc
    AVFrame* encodeFrame;
    if(swrContext){
        //LOGI("exist swrContext");
        const uint8_t** in=   (const uint8_t**)inputFrame->data;
        int ret=swr_convert(swrContext, &swrBuffer, avCodecContext->frame_size,
                    in , inputFrame->nb_samples);
        if (ret<0){
            LOGE("swr_convert error return %d",ret);
            return -1;
        }
        int length=avCodecContext->frame_size * av_get_bytes_per_sample(avCodecContext->sample_fmt);
        for (int j = 0; j < length; ++j) {
            //crash
            swrFrame->data[0][j]=(&swrBuffer)[0][j];
        }
        //LOGE("swr_convert complete");
        encodeFrame=swrFrame;
    }else{
        encodeFrame=inputFrame;
    }


    encodeFrame->pts=mAudioNextPts;
    mAudioNextPts+=encodeFrame->nb_samples;


    //LOGI("avcodec_send_frame");
    int ret=avcodec_send_frame(avCodecContext,encodeFrame);
    if (ret<0){
        LOGE("avcodec_send_frame error return %d",ret);
        return -1;
    }
    while (ret>=0){
        ret=avcodec_receive_packet(avCodecContext,&pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            //LOGI("avcodec_receive_packet eagain || eof %d",ret);
            return 1;
        }else if (ret<0){
            LOGE("avcodec_receive_packet error return %d",ret);
            return -1;
        }
        //writeAACPacketToFile(pkt.data,pkt.size);
        if (encodeFrame&& encodeFrame->pts!=AV_NOPTS_VALUE){

           // LOGI("avaudio:av_rescale_q");
            pkt.pts= av_rescale_q(encodeFrame->pts,avCodecContext->time_base,timeBase);
        }

        //LOGI("avaudio:%ld,duration:%ld",pkt.pts,pkt.duration);
        AudioPacket* outPacket=new AudioPacket();
        outPacket->data=new byte[pkt.size];
        memcpy(outPacket->data,pkt.data,pkt.size);
        outPacket->size=pkt.size;
        outPacket->duration=pkt.duration;
        outPacket->pts=pkt.pts;
        outPacket->dts=pkt.pts;
        //从开始到现在经过的毫秒数输出了多少个frame
        outPacket->position=(pkt.pts * av_q2d(timeBase) * 1000.0f);

        mAudioPacketCollector(outPacket,mAudioPacketCollectorCtx);

        //释放avPacket
        av_packet_unref(&pkt);
    }

    return 0;

}



int AudioEncoder::allocAudioStream(const char *codecName) {

    AVCodec* codec=avcodec_find_encoder_by_name(codecName);
    if (codec== nullptr){
        codec= avcodec_find_encoder( AV_CODEC_ID_AAC);
        if(codec== nullptr){
            return -1;
        }

    }
    avCodecContext = avcodec_alloc_context3(codec);
    if (avCodecContext == nullptr){
        LOGI("avcodec_alloc_context3 error");
        return -1;
    }

   // avCodecContext->codec_id=codec->id;
    avCodecContext->codec_type= AVMEDIA_TYPE_AUDIO;
    avCodecContext->sample_rate=mSampleRate;

    if (mBitRate<=0){
        mBitRate=BITE_RATE;
    }
    avCodecContext->bit_rate=mBitRate;
    //设置sampleFmt
    avCodecContext->sample_fmt=AV_SAMPLE_FMT_S16;
    //设置通道信息 输出为双声道
    avCodecContext->channel_layout=AV_CH_LAYOUT_STEREO;//mChannels==1?AV_CH_LAYOUT_MONO:AV_CH_LAYOUT_STEREO;
    avCodecContext->channels= av_get_channel_layout_nb_channels(avCodecContext->channel_layout);
    // encoding: Set by user.
    // decoding: Set by libavcodec.
    avCodecContext->profile = FF_PROFILE_AAC_LOW;
    //* Place global headers in extradata instead of every keyframe.
    avCodecContext->flags |=AV_CODEC_FLAG_GLOBAL_HEADER;





    if (mRecordChannels !=avCodecContext->channels
        || mSampleRate!=avCodecContext->sample_rate
        || AV_SAMPLE_FMT_S16!=avCodecContext->sample_fmt){


        swrContext=swr_alloc_set_opts(nullptr,
                                                  av_get_default_channel_layout(avCodecContext->channels),
                                                  AV_SAMPLE_FMT_S16,avCodecContext->sample_rate,
                                                  av_get_default_channel_layout(mRecordChannels),
                                      avCodecContext->sample_fmt,
                                                  mSampleRate,
                                                  0, nullptr
                                                  );
        if (!swrContext|| swr_init(swrContext)){
            if (swrContext){
                swr_free(&swrContext);
            }
            return -1;
        }
    }


    //试试能不能打开编解码器
    int ret=avcodec_open2(avCodecContext,codec, nullptr);
    if (ret<0){
        LOGI("can't open codec,please check");
        return -1;
    }
    //设置timebase
    avCodecContext->time_base.num=1;
    avCodecContext->time_base.den=avCodecContext->sample_rate;
    avCodecContext->frame_size=1024;//每个frame的大小



    return 0;

}

int AudioEncoder::allocAvFrame() {

    inputFrame=av_frame_alloc();
    if (!inputFrame){
        LOGI("can't alloc avframe");
        return -1;
    }
    /**
    * number of audio samples (per channel) described by this frame
    */
    inputFrame->nb_samples=avCodecContext->frame_size;
    /**
     * format of the frame, -1 if unknown or unset
     * Values correspond to enum AVPixelFormat for video frames,
     * enum AVSampleFormat for audio)
     */
    inputFrame->format=AV_SAMPLE_FMT_S16;
    inputFrame->channel_layout=AV_CH_LAYOUT_STEREO;// mChannels==1?AV_CH_LAYOUT_MONO:AV_CH_LAYOUT_STEREO;
    inputFrame->sample_rate=mSampleRate;
    /**
     * Get the required buffer size for the given audio parameters.
     */
    int bufferSize=av_samples_get_buffer_size(nullptr, av_get_channel_layout_nb_channels(inputFrame->channel_layout), inputFrame->nb_samples,
                               AV_SAMPLE_FMT_S16, 0);

    uint8_t* samples= static_cast<uint8_t *>(av_malloc(bufferSize));

    if(!samples){
        LOGI("can't allocate bytes for samples");
        return -1;
    }
    /**
     * Fill AVFrame audio data and linesize pointers.
     */
    int ret=avcodec_fill_audio_frame(inputFrame, av_get_channel_layout_nb_channels(inputFrame->channel_layout),AV_SAMPLE_FMT_S16,samples,bufferSize,0);
    if (ret<0){
        LOGI("avcodec_fill_audio_frame error");
        return -1;
    }
    if(swrContext){
        /**
         * Check if the sample format is planar.
         * @param sample_fmt the sample format to inspect
         * @return 1 if the sample format is planar, 0 if it is interleaved
         */
        //av_sample_fmt_is_planar()


        swrBufferSize=av_samples_get_buffer_size(nullptr,avCodecContext->channels,avCodecContext->frame_size,avCodecContext->sample_fmt,0);
        swrBuffer= static_cast<uint8_t *>(av_malloc(swrBufferSize));

        swrFrame=av_frame_alloc();
        if (swrFrame== nullptr){
            LOGI("av_frame_alloc error");
            return -1;
        }
        swrFrame->nb_samples=avCodecContext->frame_size;
        swrFrame->format=avCodecContext->sample_fmt;
        swrFrame->channel_layout=avCodecContext->channels==1?AV_CH_LAYOUT_MONO:AV_CH_LAYOUT_STEREO;
        swrFrame->sample_rate=avCodecContext->sample_rate;
        ret=avcodec_fill_audio_frame(swrFrame,avCodecContext->channels,avCodecContext->sample_fmt,swrBuffer,swrBufferSize,0);
        if (ret<0){
            LOGI("avcodec_fill_audio_frame error");
            return -1;
        }
    }
    nbSamples = avCodecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE ? 10240 : avCodecContext->frame_size;
    return 0;
}

void AudioEncoder::destroy() {
    if (swrBuffer!= nullptr){
        av_freep(&swrBuffer);
        swrBuffer= nullptr;
    }
    if (swrContext!= nullptr){
        swr_free(&swrContext);
        swrContext= nullptr;
    }


    if (swrFrame!= nullptr){
        av_frame_free(&swrFrame);
    }

    if (inputFrame!= nullptr){
        av_frame_free(&inputFrame);
    }


    if (avCodecContext!= nullptr){
        avcodec_close(avCodecContext);
        av_free(avCodecContext);
    }


}

void AudioEncoder::setAudioPacketCollector(AudioPacketCollector audioPacketCollector, void *ctx) {
    mAudioPacketCollector=audioPacketCollector;
    mAudioPacketCollectorCtx=ctx;
}





