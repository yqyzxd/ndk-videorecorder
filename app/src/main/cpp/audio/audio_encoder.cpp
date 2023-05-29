//
// Created by wind on 2023/4/24.
//

#include "audio_encoder.h"


#define LOG_TAG "AudioEncoder"
int AudioEncoder::init(int bitRate, int channels, int sampleRate, int bitsPerSample,
                       const char *aacFilePath, const char *codecName) {

    swrBuffer= nullptr;
    swrContext= nullptr;
    convertData= nullptr;
    swrFrame= nullptr;
    inputFrame= nullptr;
    samples= nullptr;
    avCodecContext= nullptr;
    audioStream= nullptr;
    isWriteHeaderSuccess= false;

    this->bitRate=bitRate;
    this->channels=channels;
    this->sampleRate=sampleRate;

    outputAACPath=new char[strlen(aacFilePath)+1];
    strcpy(outputAACPath,aacFilePath);

    outputAACFile= fopen(aacFilePath,"wb+");

    avFormatContext = avformat_alloc_context();

    // Allocate an AVFormatContext for an output format.
    // avformat_free_context() can be used to free the context and
    // everything allocated by the framework within it.
    int ret = avformat_alloc_output_context2(&avFormatContext, nullptr, nullptr, aacFilePath);
    if (ret < 0) {
        return -1;
    }

    //Create and initialize a AVIOContext for accessing the
    //resource indicated by url.
    ret = avio_open2(&avFormatContext->pb, aacFilePath, AVIO_FLAG_WRITE, nullptr, nullptr);
    if (ret < 0) {
        return -1;
    }
    //创建AudioStream
    ret=allocAudioStream(codecName);
    if (ret < 0) {
        LOGI("allocAudioStream error");
        return -1;
    }
    //Print detailed information about the input or output format, such as
    //duration, bitrate, streams, container, programs, metadata, side data,
    //codec and time base.
    av_dump_format(avFormatContext, 0, aacFilePath, 1);

    ret = avformat_write_header(avFormatContext, nullptr);
    if (ret < 0) {
        LOGI("avformat_write_header error");
        return -1;
    }
    isWriteHeaderSuccess=true;

    //创建AvFrame
    allocAvFrame();

    return 1;
}

void AudioEncoder::encode(byte *buffer, int size) {

    int bufferCursor=0;

    while (size>=bufferSize-samplesCursor){
        int copySize=bufferSize-samplesCursor;
        memcpy(samples+samplesCursor,buffer+bufferCursor,copySize);
        bufferCursor+=copySize;
        size-=copySize;

        long long beginEncodeTimestamp=getCurrentTimestamp();
        encodePacket();
        totalEncodeTimeMills+=getCurrentTimestamp()-beginEncodeTimestamp;
        samplesCursor=0;
    }

    if (size>0){
        memcpy(samples+samplesCursor,buffer+bufferCursor,size);
        samplesCursor+=size;
    }

}

void AudioEncoder::encodePacket() {
    AVPacket pkt;
    av_init_packet(&pkt);
    //av_packet_alloc
    AVFrame* encodeFrame;
    if(swrContext){
        LOGI("exist swrContext");
        long long beginSWRTimestamp=getCurrentTimestamp();
        const uint8_t** in=   (const uint8_t**)inputFrame->data;
        swr_convert(swrContext, convertData, avCodecContext->frame_size,
                    in , avCodecContext->frame_size);
        int length=avCodecContext->frame_size * av_get_bytes_per_sample(avCodecContext->sample_fmt);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < length; ++j) {
                swrFrame->data[i][j]=convertData[i][j];
            }
        }
        totalSWRTimeMills+=(getCurrentTimestamp()-beginSWRTimestamp);
        encodeFrame=swrFrame;
    }else{
        encodeFrame=inputFrame;
    }

    pkt.stream_index=0;
    pkt.duration=AV_NOPTS_VALUE;
    pkt.pts=pkt.dts=0;
    pkt.data=samples;
    pkt.size=bufferSize;

    //@deprecated use avcodec_send_frame()/avcodec_receive_packet() instead
    // avcodec_encode_audio2()
    LOGI("avcodec_send_frame");
    int ret=avcodec_send_frame(avCodecContext,encodeFrame);
    if (ret<0){
        LOGE("avcodec_send_frame error return %d",ret);
        return;
    }
    while (ret>=0){
        ret=avcodec_receive_packet(avCodecContext,&pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            LOGI("avcodec_receive_packet eagain || eof %d",ret);
            return;
        }else if (ret<0){
            LOGE("avcodec_receive_packet error return %d",ret);
            return;
        }
        //writeAACPacketToFile(pkt.data,pkt.size);
        if (avCodecContext->coded_frame&& avCodecContext->coded_frame->pts!=AV_NOPTS_VALUE){
            //todo pts为什么是这么设置？ a * bq / cq
            pkt.pts= av_rescale_q(avCodecContext->coded_frame->pts,avCodecContext->time_base,audioStream->time_base);
        }
        //为什么要加flags
        pkt.flags |=AV_PKT_FLAG_KEY;
        //
        this->duration=pkt.pts * av_q2d(audioStream->time_base);
        //此函数负责交错地输出一个媒体包。如果调用者无法保证来自各个媒体流的包正确交错，则最好调用此函数输出媒体包，反之，可以调用av_write_frame以提高性能。
        av_interleaved_write_frame(avFormatContext,&pkt);
        LOGI("av_interleaved_write_frame");
        //释放avPacket
        av_packet_unref(&pkt);
    }

}
void AudioEncoder::writeAACPacketToFile(uint8_t *data, int size) {
    uint8_t* buffer=new uint8_t[size+7];
    memset(buffer,0,size+1);
    memcpy(buffer+7,data,size);
    addADTSToPacket(buffer,size+7);
    fwrite(buffer,sizeof(uint8_t),size+7,outputAACFile);

}

void AudioEncoder::addADTSToPacket(uint8_t *buffer, int size) {
    int profile = 29;//2 : LC; 5 : HE-AAC; 29: HEV2
    int freqIdx = 3; // 48KHz  44100对应的值是4
    int chanCfg = 1; // Mono

    // fill in ADTS data
    buffer[0] = (unsigned char) 0xFF;
    buffer[1] = (unsigned char) 0xF1;
    buffer[2] = (unsigned char) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2)); //(unsigned char) 0x58;
    buffer[3] = (unsigned char) (((chanCfg & 3) << 6) + (size >> 11));
    buffer[4] = (unsigned char) ((size & 0x7FF) >> 3);
    buffer[5] = (unsigned char) (((size & 7) << 5) + 0x1F);
    buffer[6] = (unsigned char) 0xFC;
}

int AudioEncoder::allocAudioStream(const char *codecName) {
    audioStream=avformat_new_stream(avFormatContext, nullptr);
    /**
   * Format-specific stream ID.
   * decoding: set by libavformat
   * encoding: set by the user, replaced by libavformat if left unset
   */
    audioStream->id=1;

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

    avCodecContext->codec_id=codec->id;
    avCodecContext->codec_type= AVMEDIA_TYPE_AUDIO;
    avCodecContext->sample_rate=sampleRate;

    if (bitRate<=0){
        bitRate=BITE_RATE;
    }
    avCodecContext->bit_rate=bitRate;
    //设置sampleFmt
    avCodecContext->sample_fmt=AV_SAMPLE_FMT_S16;
    //设置通道信息
    avCodecContext->channel_layout=channels==1?AV_CH_LAYOUT_MONO:AV_CH_LAYOUT_STEREO;
    avCodecContext->channels= av_get_channel_layout_nb_channels(avCodecContext->channel_layout);
    // encoding: Set by user.
    // decoding: Set by libavcodec.
    avCodecContext->profile = FF_PROFILE_AAC_HE;
    //* Place global headers in extradata instead of every keyframe.
    avCodecContext->flags |=AV_CODEC_FLAG_GLOBAL_HEADER;






    //设置最合适的sampleFormat
    //array of supported sample formats, or NULL if unknown, array is terminated by -1
    const enum  AVSampleFormat* avSampleFormats=codec->sample_fmts;
    if (avSampleFormats){
        for (; *avSampleFormats!=-1 ; avSampleFormats++) {
            if (*avSampleFormats == avCodecContext->sample_fmt){
                break;
            }
        }
        if ( *avSampleFormats==-1){
            LOGI("sample fmt not supported,user the codec supported sample fmt ");
            avCodecContext->sample_fmt=codec->sample_fmts[0];
        }
    }

    //找到最合适的采样率
    if (codec->supported_samplerates){
        //array of supported audio samplerates, or NULL if unknown, array is terminated by 0
        const int *sampleRates=codec->supported_samplerates;
        int best=0;
        int best_dist=INT_MAX;
        for (; *sampleRates ; sampleRates++) {
            int dist= abs(avCodecContext->sample_rate-*sampleRates);
            if (dist<best_dist){
                best_dist=dist;
                best=*sampleRates;
            }
        }

        avCodecContext->sample_rate=best;
    }

    if (channels !=avCodecContext->channels
        || sampleRate!=avCodecContext->sample_rate
        || AV_SAMPLE_FMT_S16!=avCodecContext->sample_fmt){
        LOGI("channels is {%d, %d}", avCodecContext->channels, audioStream->codec->channels);
        LOGI("sample_rate is {%d, %d}", avCodecContext->sample_rate, audioStream->codec->sample_rate);
        LOGI("sample_fmt is {%d, %d}", avCodecContext->sample_fmt, audioStream->codec->sample_fmt);
        LOGI("AV_SAMPLE_FMT_S16P is %d AV_SAMPLE_FMT_S16 is %d AV_SAMPLE_FMT_FLTP is %d", AV_SAMPLE_FMT_S16P, AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_FLTP);

        swrContext=swr_alloc_set_opts(nullptr,
                                                  av_get_default_channel_layout(avCodecContext->channels),
                                                  avCodecContext->sample_fmt,avCodecContext->sample_rate,
                                                  av_get_default_channel_layout(channels),
                                                  AV_SAMPLE_FMT_S16,
                                                  sampleRate,
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

    AVCodecParameters* avCodecParameters=audioStream->codecpar;

    //将avCodecContext中的信息拷贝到avCodecParameters
    ret=avcodec_parameters_from_context(avCodecParameters,avCodecContext);
    if (ret<0){
        LOGI("avcodec_parameters_from_context failed,please check");
        return -1;
    }


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
    inputFrame->channel_layout= channels==1?AV_CH_LAYOUT_MONO:AV_CH_LAYOUT_STEREO;
    inputFrame->sample_rate=sampleRate;
    /**
     * Get the required buffer size for the given audio parameters.
     */
    bufferSize=av_samples_get_buffer_size(nullptr, av_get_channel_layout_nb_channels(inputFrame->channel_layout), inputFrame->nb_samples,
                               AV_SAMPLE_FMT_S16, 0);

    samples= static_cast<uint8_t *>(av_malloc(bufferSize));
    samplesCursor=0;
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

        convertData= static_cast<uint8_t **>(calloc(avCodecContext->channels, sizeof(*convertData)));
        /**
         * Allocate a samples buffer for nb_samples samples, and fill data pointers and
         * linesize accordingly.
         * The allocated samples buffer can be freed by using av_freep(&audio_data[0])
         * Allocated data will be initialized to silence.
         */
        ret=av_samples_alloc(convertData, nullptr,avCodecContext->channels,avCodecContext->frame_size,avCodecContext->sample_fmt,0);
        if (ret<0){
            LOGI("av_samples_alloc error");
        }
        swrBufferSize=av_samples_get_buffer_size(nullptr,avCodecContext->channels,avCodecContext->frame_size,avCodecContext->sample_fmt,0);
        swrBuffer= static_cast<uint8_t *>(av_malloc(swrBufferSize));

        swrFrame=av_frame_alloc();
        if (swrFrame== nullptr){
            LOGI("av_frame_alloc error");
            return -1;
        }
        swrFrame->nb_samples=avCodecContext->frame_size;
        swrFrame->format=avCodecContext->sample_rate;
        swrFrame->channel_layout=avCodecContext->channels==1?AV_CH_LAYOUT_MONO:AV_CH_LAYOUT_STEREO;
        swrFrame->sample_rate=avCodecContext->sample_rate;
        ret=avcodec_fill_audio_frame(swrFrame,avCodecContext->channels,avCodecContext->sample_fmt,swrBuffer,swrBufferSize,0);
        if (ret<0){
            LOGI("avcodec_fill_audio_frame error");
            return -1;
        }
    }

    return 0;
}

void AudioEncoder::destroy() {
    if (swrBuffer!= nullptr){
        av_freep(swrBuffer);
        swrBuffer= nullptr;
        swrBufferSize=0;
    }
    if (swrContext!= nullptr){
        swr_free(&swrContext);
        swrContext= nullptr;
    }
    if (convertData!= nullptr){
        av_freep(&convertData[0]);
        free(convertData);
    }

    if (swrFrame!= nullptr){
        av_frame_free(&swrFrame);
    }
    if(samples!= nullptr){
        av_freep(&samples);
    }
    if (inputFrame!= nullptr){
        av_frame_free(&inputFrame);
    }

    if (isWriteHeaderSuccess){
        avFormatContext->duration=duration*AV_TIME_BASE;
        av_write_trailer(avFormatContext);
    }
    if (avCodecContext!= nullptr){
        avcodec_close(avCodecContext);
        av_free(avCodecContext);
    }
    if (avCodecContext!= nullptr && avFormatContext->pb!= nullptr){
        avio_close(avFormatContext->pb);
    }

}





