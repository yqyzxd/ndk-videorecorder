//
// Created by wind on 2023/5/6.
//

#include "loop_thread.h"

LoopThread::LoopThread():Thread() {
    mLooper=new Looper;
}
LoopThread::~LoopThread() {
    delete mLooper;
    mLooper= nullptr;
}

void LoopThread::run() {
    mLooper->loop();
}

Looper *LoopThread::getLooper() {
    return mLooper;
}
void LoopThread::quit() {
    mLooper->quit();
}