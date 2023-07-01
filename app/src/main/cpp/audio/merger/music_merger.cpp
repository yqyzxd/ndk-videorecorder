//
// Created by 史浩 on 2023/6/24.
//

#include "music_merger.h"
#include "../../utils/types.h"
#include "../../utils/log.h"

#define LOG_TAG "MusicMerger"
MusicMerger::MusicMerger(){

}
MusicMerger::~MusicMerger() {

}

int MusicMerger::mergeMusic(short *accompanySamples, int accompanySize,int* accompanySamplesCursor,
                            short *audioSamples,int audioSize,int* audioSamplesCursor) {

    int ret=0;
    int accompanyRemainSize=accompanySize-*accompanySamplesCursor;
    int audioRemainSize=audioSize-*audioSamplesCursor;

    int mergeSize=audioRemainSize;
    if(audioRemainSize>=accompanyRemainSize){
        mergeSize=accompanyRemainSize;
        if(audioRemainSize==accompanyRemainSize){
            ret=0;
        } else{
            ret=-2;//means need more
        }

    }


    for (int i=0;i<mergeSize;i++){
        int audioPosition=*audioSamplesCursor+i;
        short audio=audioSamples[audioPosition];

        int accompanyPosition=*accompanySamplesCursor+i;
        short accompany=accompanySamples[accompanyPosition];
        audioSamples[audioPosition]=mixSamples(accompany,audio);
        //audioSamples[audioPosition]=accompany;

    }

    int newAccompanySamplesCursor=(*accompanySamplesCursor+mergeSize);
    *accompanySamplesCursor=newAccompanySamplesCursor;

    int newAudioSamplesCursor=(*audioSamplesCursor+mergeSize);
    *audioSamplesCursor=newAudioSamplesCursor;
    return ret;
}
//音频混音 https://blog.csdn.net/u010164190/article/details/117691952
short MusicMerger::mixSamples(short a, short b) {

   // int aInt=(int)a;
   // int bInt=(int)b;
    short tmp=(a+b)/2;
   /* if(a<0&&b<0){
         tmp=(aInt+bInt)- (aInt*bInt)/INT16_MIN;
    }
    if(a>0&& b>0){
        tmp=(aInt+aInt)-aInt*bInt/INT16_MAX;
    }*/

    if(tmp>INT16_MAX){
        LOGE("mixSamples>INT16_MAX");
       tmp= INT16_MAX;
    }
    if(tmp<INT16_MIN){
        LOGE("mixSamples<INT16_MIN");
        tmp=INT16_MIN;
    }
    return tmp;
}
