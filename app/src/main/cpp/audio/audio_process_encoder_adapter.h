//
// Created by 史浩 on 2023/6/24.
//

#ifndef NDK_VIDEORECORDER_AUDIO_PROCESS_ENCODER_ADAPTER_H
#define NDK_VIDEORECORDER_AUDIO_PROCESS_ENCODER_ADAPTER_H

#include "audio_encoder_adapter.h"
#include "merger/music_merger.h"

//将伴奏和录音合并成一轨
class AudioProcessEncoderAdapter: public AudioEncoderAdapter{
public:
    AudioProcessEncoderAdapter();
    virtual ~AudioProcessEncoderAdapter();

    int init(int audioBitrate,int audioSampleRate, int audioChannels);
    void dealloc();
protected:
     void discardAudioPacket();
     int processAudio();

private:
    VideoPacketPool* mAccompanyPool;
    MusicMerger* mMerger;
    AudioFrame* mAccompanyFrame;
    int mAccompanyCursor;
};


#endif //NDK_VIDEORECORDER_AUDIO_PROCESS_ENCODER_ADAPTER_H
