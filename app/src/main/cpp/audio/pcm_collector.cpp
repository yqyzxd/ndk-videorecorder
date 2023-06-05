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
        mAudioEncoderAdapter=new AudioEncoderAdapter();
        //int audioBitrate,int audioSampleRate, int audioChannels
        int audioBitrate=0;
        int audioChannels=2;
        mAudioEncoderAdapter->init(audioBitrate,mAudioSampleRate,audioChannels);
        LOGI("after audio encoder adapter init");
    }
    //LOGI("pcm collector collect");
    while (sizeInShort>0) {
        int remainSize = mBufferSizeInShort - mBufferCursor;
        if (remainSize >= sizeInShort) {
            //有剩余空间存放
            //memcpy 的第三个参数size单位是字节，而我们的数据是short类型，所以*2
            memcpy(mAudioBuffer, data, sizeInShort * 2);
            mBufferCursor += sizeInShort;
            sizeInShort = 0;
        } else {
            //剩余空间不够了

            memcpy(mAudioBuffer, data, remainSize * 2);
            mBufferCursor=mBufferSizeInShort;
            sizeInShort -= remainSize;
            //insert to queue
            enqueue();
            mBufferCursor = 0;

        }
    }

}

void PcmCollector::enqueue() {
    if (mBufferCursor>0){
        AudioPacket* packet=new AudioPacket ;

        short* packetBuffer=new short[mBufferCursor];
        memcpy(packetBuffer,mAudioBuffer,mBufferCursor*sizeof(short));

        packet->buffer=packetBuffer;
        packet->size=mBufferCursor;
        packet->position=currentTimeMills()-mStartTimeMillis;
        mPool->enqueueAudioFrame(packet);
        LOGI("enqueueAudioFrame");
        //todo 检测时间戳，是否需要插入空数据
    }

}

void PcmCollector::stop() {
    //剩余数据插入队列
    enqueue();
    mStartTimeMillis=0;
    delete[] mAudioBuffer;

    if (mAudioEncoderAdapter!= nullptr){
        mAudioEncoderAdapter->dealloc();
    }


}
