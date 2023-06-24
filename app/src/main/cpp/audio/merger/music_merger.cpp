//
// Created by 史浩 on 2023/6/24.
//

#include "music_merger.h"
#include "../../utils/types.h"

MusicMerger::MusicMerger(){

}
MusicMerger::~MusicMerger() {

}

int MusicMerger::mergeMusic(short *accompanySamples, int accompanySize, short *audioSamples,
                            int audioSize) {

    //todo 为什么要这样做？
    for (int i=audioSize-1;i>=0;i--) {
        audioSamples[i]=audioSamples[i/2];
    }

    int actualSize=audioSize;
    if(audioSize>accompanySize){
        actualSize=accompanySize;
    }


    for (int i=0;i<actualSize;i++){
        short audio=audioSamples[i];
        short accompany=accompanySamples[i];
        audioSamples[i]=mixSamples(accompany,audio);
    }

    return actualSize;
}

short MusicMerger::mixSamples(short a, short b) {

    int aInt=(int)a;
    int bInt=(int)b;
    int tmp=a+b;
    if(a<0&&b<0){
         tmp=(aInt+bInt)- (aInt*bInt)/INT16_MIN;
    }
    if(a>0&& b>0){
        tmp=(aInt+aInt)-aInt*bInt/INT16_MAX;
    }

    if(tmp>INT16_MAX){
       tmp= INT16_MAX;
    }
    if(tmp<INT16_MIN){
        tmp=INT16_MIN;
    }
    return tmp;
}
