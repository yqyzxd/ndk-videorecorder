//
// Created by wind on 2023/5/6.
//

#include "handler.h"

Handler::Handler(Looper *looper):mLooper(looper) {

}

Handler::~Handler() {

}

void Handler::sendMessage(Message *message) {
    message->target=this;
    mLooper->getQueue()->enqueue(message);
}