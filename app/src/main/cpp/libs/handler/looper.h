//
// Created by wind on 2023/5/6.
//

#ifndef NDK_HANDLER_LOOPER_H
#define NDK_HANDLER_LOOPER_H


#include "message_queue.h"

class Looper {
private:
    MessageQueue* mQueue;
public:
    Looper();
    ~Looper();

    MessageQueue* getQueue();

    void loop();
    void quit();
};


#endif //NDK_HANDLER_LOOPER_H
