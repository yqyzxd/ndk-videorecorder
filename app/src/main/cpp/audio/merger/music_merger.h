//
// Created by 史浩 on 2023/6/24.
//

#ifndef NDK_VIDEORECORDER_MUSIC_MERGER_H
#define NDK_VIDEORECORDER_MUSIC_MERGER_H

//将伴奏和录音合并成一轨
class MusicMerger {
public:
    MusicMerger();
    ~MusicMerger();

    int mergeMusic(short* accompanySamples,int accompanySize,short* audioSamples,int audioSize);
private:
    short mixSamples(short accompany, short audio);
};


#endif //NDK_VIDEORECORDER_MUSIC_MERGER_H
