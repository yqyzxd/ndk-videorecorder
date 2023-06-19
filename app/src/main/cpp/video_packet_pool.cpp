//
// Created by wind on 2023/5/24.
//

#include "video_packet_pool.h"


VideoPacketPool::VideoPacketPool() {

    pthread_rwlock_init(&mRWLock, nullptr);
}

VideoPacketPool::~VideoPacketPool() {
    pthread_rwlock_destroy(&mRWLock);
}

void VideoPacketPool::initPool() {
    mVideoPktQueue = new LinkedBlockingQueue<VideoPacket *>();
    mAudioFrameQueue = new LinkedBlockingQueue<AudioFrame *>();
    mAudioPktQueue = new LinkedBlockingQueue<AudioPacket *>();
    mTotalDiscardVideoPacketDuration=0;
}

VideoPacketPool *VideoPacketPool::instance = new VideoPacketPool();

VideoPacketPool *VideoPacketPool::getInstance() {
    return instance;
}

int VideoPacketPool::enqueueVideoPacket(VideoPacket *packet) {
    if (mVideoPktQueue != nullptr) {
        //检测是否需要进行丢帧
        if (mVideoPktQueue->size()>THRESHOLD_VIDEO_PACKET_QUEUE){
            LOGI("enqueueVideoPacket() size:%d",mVideoPktQueue->size());
        }

        //todo 存在卡住线程的情况，需要检查
        while (detectDiscardVideoPacket()) {
            LOGI("detectDiscardVideoPacket() ture");
            int countOfDiscardPackets = 0;
            int durationOfDiscardPackets = 0;

            int ret = discardVideoGOP(&countOfDiscardPackets, &durationOfDiscardPackets);
            LOGI("discardVideoGOP() return");
            if (ret < 0) {
                //discard error
                break;
            }
            recordDropVideoFrame(durationOfDiscardPackets);
            LOGI("discard packets :%d,duration:%d", countOfDiscardPackets,
                 durationOfDiscardPackets);
        }
        if (mCurVideoPacket != nullptr) {
            int duration = packet->timeMills - mCurVideoPacket->timeMills;
            mCurVideoPacket->duration = duration;
            mVideoPktQueue->put(mCurVideoPacket);
            //LOGI("enqueueVideoPacket：%d",mVideoPktQueue->size());
        }
        mCurVideoPacket = packet;
    }

    return 0;
}

bool VideoPacketPool::detectDiscardVideoPacket() {
    return mVideoPktQueue->size() > THRESHOLD_VIDEO_PACKET_QUEUE;
}

int VideoPacketPool::discardVideoGOP(int *countOfDiscardPackets, int *durationOfDiscardPackets) {
    VideoPacket *videoPacket;
    *countOfDiscardPackets = 0;
    *durationOfDiscardPackets = 0;

    int ret = mVideoPktQueue->peek(&videoPacket);
    bool isFirstPacket = true;
    while (ret >= 0) {
        LOGI("discardVideoGOP()");
        if (isFirstPacket) {
            isFirstPacket = false;
            if (videoPacket->getNALUType() == NALU_TYPE_IDR) {
                //队列中的第一个packet刚好是IDR帧
                ret = mVideoPktQueue->take(&videoPacket);
                delete videoPacket;
                (*countOfDiscardPackets)++;
                *durationOfDiscardPackets += videoPacket->duration;
            }
        } else {
            ret = mVideoPktQueue->peek(&videoPacket);
            if (videoPacket->getNALUType() == NALU_TYPE_IDR) {
                break;
            } else {
                mVideoPktQueue->take(&videoPacket);
                delete videoPacket;
                (*countOfDiscardPackets)++;
                *durationOfDiscardPackets += videoPacket->duration;
            }
        }
    }
    return 0;
}

int VideoPacketPool::getVideoPacket(VideoPacket **packet) {
    return mVideoPktQueue->take(packet);
}

int VideoPacketPool::getVideoPacketQueueSize() {
    return mVideoPktQueue->size();
}

int VideoPacketPool::abortVideoPacketQueue() {
    mVideoPktQueue->flush();
    return 0;
}

int VideoPacketPool::enqueueAudioFrame(AudioFrame *packet) {
    return mAudioFrameQueue->put(packet);
}

bool VideoPacketPool::detectDiscardAudioFrame() {
    bool ret=false;
    pthread_rwlock_rdlock(&mRWLock);
    ret=mTotalDiscardVideoPacketDuration >= AUDIO_PACKET_DURATION_IN_SECS *1000.0f;
    pthread_rwlock_unlock(&mRWLock);

    return ret;
}

void VideoPacketPool::recordDropVideoFrame(int discardDuration) {

    pthread_rwlock_wrlock(&mRWLock);
    mTotalDiscardVideoPacketDuration+=discardDuration;
    pthread_rwlock_unlock(&mRWLock);
}

bool VideoPacketPool::discardAudioFrame() {
    int ret=0;
    AudioFrame *packet;
    ret= mAudioFrameQueue->take(&packet);
    if (ret<0){
        return false;
    }
    delete packet;
    packet= nullptr;

    pthread_rwlock_wrlock(&mRWLock);
    mTotalDiscardVideoPacketDuration-=(AUDIO_PACKET_DURATION_IN_SECS *1000.0f);
    pthread_rwlock_unlock(&mRWLock);
    return true;
}

int VideoPacketPool::getAudioFrame(AudioFrame **packet) {
    return mAudioFrameQueue->take(packet);
}
int VideoPacketPool::abortAudioFrameQueue() {
    mAudioFrameQueue->flush();
    return 0;
}

int VideoPacketPool::enqueueAudioPacket(AudioPacket *packet) {
    return mAudioPktQueue->put(packet);
}

int VideoPacketPool::getAudioPacketQueueSize() {
    return mAudioPktQueue->size();
}

int VideoPacketPool::getAudioPacket(AudioPacket **packet) {
    return mAudioPktQueue->take(packet);
}

int VideoPacketPool::abortAudioPacketQueue() {
     mAudioPktQueue->flush();
     return 0;
}


bool VideoPacketPool::dealloc() {
    delete mVideoPktQueue;
    delete mAudioPktQueue;
    delete mAudioFrameQueue;
}