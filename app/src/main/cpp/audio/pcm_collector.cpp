//
// Created by wind on 2023/5/30.
//

#include "pcm_collector.h"


PcmCollector::PcmCollector() {



}

int PcmCollector::init(int audioSampleRate, int bufferSizeInShort) {
    this->mAudioSampleRate=audioSampleRate;
    this->mBufferSizeInShort=bufferSizeInShort;
    mPool=VideoPacketPool::getInstance();
    mStartTimeMillis=0;
    mAudioBuffer=new short[mBufferSizeInShort];

    mAudioEncoderAdapter= nullptr;
    return 0;
}



PcmCollector::~PcmCollector() {

}

void PcmCollector::collect(short *data, int sizeInShort) {
    if(sizeInShort<=0){
        return;
    }
    if (mStartTimeMillis==0){
        mStartTimeMillis= currentTimeMills();
        mAudioEncoderAdapter=new AudioProcessEncoderAdapter();


        int audioBitrate=0;
        int recordAudioChannels=1;
        mAudioEncoderAdapter->init(audioBitrate,mAudioSampleRate,recordAudioChannels);
        LOGI("after audio encoder adapter init");
    }

    int dataCursor = 0;
    int samplesCnt =sizeInShort;
    while (samplesCnt>0) {
        if ((mBufferCursor+dataCursor) < mBufferSizeInShort) {
            //有剩余空间存放
            //memcpy 的第三个参数size单位是字节，而我们的数据是short类型，所以*2
            memcpy(mAudioBuffer+mBufferCursor, data+dataCursor, sizeInShort * 2);
            mBufferCursor += samplesCnt;
            dataCursor+=samplesCnt;
            samplesCnt = 0;
        } else {
            int remainSize = mBufferSizeInShort - mBufferCursor;
            //剩余空间不够了
            memcpy(mAudioBuffer+mBufferCursor, data+dataCursor, remainSize * 2);
            mBufferCursor=mBufferSizeInShort;
            samplesCnt -= remainSize;
            dataCursor+=remainSize;
            //insert to queue
            enqueue();
            mBufferCursor = 0;
        }
    }

}

void PcmCollector::enqueue() {
    if (mBufferCursor>0){
        AudioFrame* frame=new AudioFrame ;

        short* packetBuffer=new short[mBufferCursor];
        memcpy(packetBuffer,mAudioBuffer,mBufferCursor*sizeof(short));

        frame->buffer=packetBuffer;
        frame->size=mBufferCursor;
        frame->position=currentTimeMills()-mStartTimeMillis;
        mPool->enqueueAudioFrame(frame);
        //LOGI("enqueueAudioFrame");
        //todo 检测时间戳，是否需要插入空数据
    }

}

void PcmCollector::stop() {
    //剩余数据插入队列
    //enqueue();


    if (mAudioEncoderAdapter!= nullptr){
        mAudioEncoderAdapter->dealloc();
        mAudioEncoderAdapter= nullptr;
    }

    mStartTimeMillis=0;
    if (mAudioBuffer!= nullptr){
        delete[] mAudioBuffer;
        mAudioBuffer= nullptr;
    }

}
