//
// Created by wind on 2023/5/6.
//

#include "message_queue.h"

MessageQueue::MessageQueue() {
    mQuit= false;
    mMessages= nullptr;
    pthread_mutex_init(&mThreadMutex,0);
    pthread_cond_init(&mThreadCond,0);
}

MessageQueue::~MessageQueue() {
    flush();
    pthread_mutex_destroy(&mThreadMutex);
    pthread_cond_destroy(&mThreadCond);
}

int MessageQueue::enqueue(Message *message) {
    pthread_mutex_lock(&mThreadMutex);

    Message* head=mMessages;
    if (head== nullptr){
        mMessages=message;
    }else{
        Message* node=head;
        //遍历到尾部
        while (node->next!= nullptr){
            node=node->next;
        }
        node->next=message;
    }
    pthread_cond_signal(&mThreadCond);
    pthread_mutex_unlock(&mThreadMutex);

    return 0;
}


Message *MessageQueue::next() {

    pthread_mutex_lock(&mThreadMutex);
    if (mQuit){
        return nullptr;
    }
    if (mMessages== nullptr){
        pthread_cond_wait(&mThreadCond,&mThreadMutex);
        if (mQuit){
            return nullptr;
        }
    }
    Message* head=mMessages;
    mMessages=head->next;

    pthread_mutex_unlock(&mThreadMutex);
    return head;
}

void MessageQueue::flush() {
    pthread_mutex_lock(&mThreadMutex);
    Message* node=mMessages;
    while (node!= nullptr){
        Message* next=node->next;
        delete node;
        node=next;
    }

    pthread_mutex_unlock(&mThreadMutex);

}

void MessageQueue::quit() {
    pthread_mutex_lock(&mThreadMutex);
    mQuit= true;
    pthread_cond_signal(&mThreadCond);
    pthread_mutex_unlock(&mThreadMutex);
}