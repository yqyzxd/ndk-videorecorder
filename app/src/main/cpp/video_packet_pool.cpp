//
// Created by wind on 2023/5/24.
//

#include "video_packet_pool.h"



VideoPacketPool::VideoPacketPool() {
    mVideoPktQueue = new LinkedBlockingQueue<VideoPacket*>();
}

VideoPacketPool::~VideoPacketPool() {}

VideoPacketPool* VideoPacketPool::instance=new VideoPacketPool();

VideoPacketPool *VideoPacketPool::getInstance() {
    return instance;
}

int VideoPacketPool::enqueueVideoPacket(VideoPacket *packet) {
    if (mVideoPktQueue != nullptr) {
        //检测是否需要进行丢帧
        if (detectDiscardVideoPacket()) {
            int countOfDiscardPackets = 0;
            int durationOfDiscardPackets = 0;


            int ret = discardVideoGOP(&countOfDiscardPackets,&durationOfDiscardPackets);
            if (ret<0){
                //discard error
            }
            LOGI("discard packets :%d,duration:%d",countOfDiscardPackets,durationOfDiscardPackets);
        }
        if (mCurVideoPacket!= nullptr){
            int duration= packet->timeMills - mCurVideoPacket->timeMills;
            mCurVideoPacket->duration=duration;
            mVideoPktQueue->put(mCurVideoPacket);
        }
        mCurVideoPacket=packet;
    }

    return 0;
}

bool VideoPacketPool::detectDiscardVideoPacket() {
    return mVideoPktQueue->size() > THRESHOLD_VIDEO_PACKET_QUEUE;
}

int VideoPacketPool::discardVideoGOP(int *countOfDiscardPackets, int *durationOfDiscardPackets) {
    VideoPacket* videoPacket;
    *countOfDiscardPackets=0;
    *durationOfDiscardPackets=0;

    int ret =mVideoPktQueue->peek(&videoPacket);
    bool isFirstPacket= true;
    while (ret>=0){

        if (isFirstPacket){
            isFirstPacket= false;
            if (videoPacket->getNALUType()==NALU_TYPE_IDR){
                //队列中的第一个packet刚好是IDR帧
                ret= mVideoPktQueue->take(&videoPacket);
                delete videoPacket;
                (*countOfDiscardPackets)++;
                *durationOfDiscardPackets+=videoPacket->duration;
            }
        }else{
            ret= mVideoPktQueue->peek(&videoPacket);
            if (videoPacket->getNALUType()==NALU_TYPE_IDR){
                break;
            }else{
                mVideoPktQueue->take(&videoPacket);
                delete videoPacket;
                (*countOfDiscardPackets)++;
                *durationOfDiscardPackets+=videoPacket->duration;
            }
        }
    }
    return 0;
}
