//
// Created by wind on 2023/5/6.
//

#ifndef NDK_HANDLER_MESSAGE_QUEUE_H
#define NDK_HANDLER_MESSAGE_QUEUE_H


#include "message.h"
#include <pthread.h>
class MessageQueue {

private:
    Message* mMessages;

    pthread_mutex_t mThreadMutex;
    pthread_cond_t mThreadCond;

    bool mQuit;

public:
    MessageQueue();
    ~MessageQueue();

    int enqueue(Message* message);
    //int dequeue(Message** message);

    Message* next();
    void quit();
    void flush();
};


#endif //NDK_HANDLER_MESSAGE_QUEUE_H
