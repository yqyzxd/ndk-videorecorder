//
// Created by wind on 2023/5/6.
//

#include "looper.h"

Looper::Looper() {
    mQueue = new MessageQueue();
}
Looper::~Looper() {

}

void Looper::loop() {

    while (true) {
        Message *message = mQueue->next();
        if (message == nullptr) {
            return;
        }
        message->execute();
    }

}

MessageQueue *Looper::getQueue() {
    return mQueue;
}
void Looper::quit() {
    mQueue->quit();
}